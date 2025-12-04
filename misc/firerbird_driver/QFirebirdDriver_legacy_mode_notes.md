# Driver Firebird Qt 6 – Mode LEGACY pour anciennes bases charset NONE

Ce document décrit :

- les problèmes rencontrés avec les anciennes bases Firebird en `charset NONE` sous Qt 6 ;
- les modifications apportées au driver `QFirebird` pour Qt 6.10 ;
- comment utiliser le nouveau mode `LEGACY` via `setConnectOptions()` ;
- le comportement détaillé en mode `MODERN` vs `LEGACY`.

---

## 1. Problème initial

### Contexte

- Application compilée avec **Qt 5.15** et **Qt 6.10**.
- Accès à une base **Firebird ancienne** dont le **charset est `NONE`**.
- Driver Firebird personnalisé :
  - sous Qt 5.15 : dérivé du `QFirebirdDriver` de Qt 5.15 ;
  - sous Qt 6.10 : dérivé du `QFirebirdDriver` de Qt 6.10.
- Avec Qt 5.15, les **caractères accentués sont correctement récupérés**.
- Avec Qt 6.10, les chaînes accentuées sont corrompues, par exemple pour `"Série 4000"` :

  ```text
  ---->Octets bruts avec OCTETS: "53 ef bf bd 72 69 65 20 34 30 30 30"
  ---->Texte Latin1: "Sï¿½rie 4000"
  ---->Texte direct: "S?rie 4000"
  ```

  `ef bf bd` est la séquence UTF-8 pour le caractère de remplacement `U+FFFD` (`�`), ce qui signifie que le `é` a déjà été remplacé avant même l’affichage (corruption en amont).

### Différences essentielles entre Qt 5.15 et Qt 6.10

1. **Qt 5.15** (driver d’origine) :

   - Si `ISC_DPB_LC_CTYPE` n’est pas fourni dans les options, le driver force :
     ```cpp
     encString = "UNICODE_FSS"
     ```
   - Le driver utilise un `QTextCodec *tc` pour gérer les conversions de chaînes.
   - Dans la pratique, sur une ancienne base `NONE`, cette combinaison fonctionnait correctement pour les données stockées en 8 bits.

2. **Qt 6.10** (driver de base) :

   - Le driver force `UTF-8` par défaut si aucun `ISC_DPB_LC_CTYPE` n’est fourni.
   - Les conversions sont faites avec `QString::fromUtf8()` / `toUtf8()` et `QStringConverter`.
   - Sur une base `NONE` contenant des octets 8 bits non UTF-8 (latin1/Win-1252, etc.), cette interprétation stricte provoque des remplacements par `�` (`ef bf bd`).

Conclusion :  
Le passage à Qt 6 s’est accompagné de changements d’encodage côté client et d’un forçage explicite de `UTF-8`, incompatibles avec les données anciennes non UTF-8 dans une base `NONE`.

---

## 2. Objectif des modifications

L’objectif est de :

1. Introduire un **mode LEGACY** sélectionnable via `setConnectOptions()` :
   - pour garder un comportement plus « brut » compatible avec une base `charset NONE` ;
   - et décoder les données comme des chaînes 8 bits (Latin1) plutôt qu’en UTF-8.

2. Garder également un **mode MODERN** (UTF-8 explicite) pour les bases récentes bien configurées.

On ajoute donc dans la chaîne de connexion une option supplémentaire :

- `REX_ENCODING_MODE=LEGACY`  
  → mode ancien pour les bases `NONE` avec données 8 bits.
- `REX_ENCODING_MODE=MODERN`  
  → mode moderne / UTF-8 (comportement Qt 6 natif).

Si `REX_ENCODING_MODE` n’est pas spécifié, le comportement reste **équivalent à MODERN**.

---

## 3. Modifications apportées au driver Qt 6 (qsql_firebird.cpp)

Les modifications concernent principalement :

- la fonction `QFirebirdDriver::open(...)` (gestion du `lc_ctype`) ;
- la fonction `QFirebirdResult::gotoNext(...)` (décodage des champs texte) ;
- l’ajout d’une map globale pour mémoriser le mode legacy par connexion.

Toutes les modifications sont faites dans le fichier **`qsql_firebird.cpp`** (et le header reste pratiquement inchangé).

### 3.1. Ajout d’une map globale pour le flag `legacyNoLcCtype`

Juste après les définitions globales existantes :

```cpp
typedef QMap<void *, QFirebirdDriver *> QFirebirdBufferDriverMap;
Q_GLOBAL_STATIC(QFirebirdBufferDriverMap, qBufferDriverMap)
Q_GLOBAL_STATIC(QMutex, qMutex);
```

on ajoute :

```cpp
// Map globale pour mémoriser le mode LEGACY "sans lc_ctype" par connexion
typedef QMap<isc_db_handle, bool> QFirebirdLegacyModeMap;
Q_GLOBAL_STATIC(QFirebirdLegacyModeMap, qLegacyNoLcCtypeMap)
```

Cette map associe un `isc_db_handle` (handle Firebird de la connexion) à un booléen :

- `true` si l’on a ouvert cette connexion en mode **LEGACY sans `lc_ctype` explicite**,
- `false` sinon.

Cette information sera utilisée à la lecture des données, pour choisir entre `fromLatin1` et `fromUtf8`.

### 3.2. Gestion de `REX_ENCODING_MODE` et de `ISC_DPB_LC_CTYPE` dans `open()`

Dans `QFirebirdDriver::open(...)`, un `enum class RexEncodingMode` existait déjà :

```cpp
enum class RexEncodingMode {
    Auto,      // comportement par défaut
    Legacy,    // UNICODE_FSS (mode Qt 5.15)
    Modern     // UTF-8 explicite
};
RexEncodingMode rexMode = RexEncodingMode::Auto;
```

et le parsing de `REX_ENCODING_MODE` dans les options :

```cpp
else if (opt.compare(u"REX_ENCODING_MODE", Qt::CaseInsensitive) == 0) {
    if (val.compare(u"LEGACY", Qt::CaseInsensitive) == 0) {
        rexMode = RexEncodingMode::Legacy;
    } else if (val.compare(u"MODERN", Qt::CaseInsensitive) == 0) {
        rexMode = RexEncodingMode::Modern;
    } else {
        qWarning("Unknown REX_ENCODING_MODE value '%s'. Expected LEGACY or MODERN. Using Auto.", qPrintable(val));
        rexMode = RexEncodingMode::Auto;
    }
}
```

Ce qui change est la **logique d’encodage** après cette boucle.

Au lieu de forcer `UNICODE_FSS` en LEGACY, nous adoptons la stratégie suivante :

- En **LEGACY** :
  - si l’utilisateur n’a **pas** spécifié `ISC_DPB_LC_CTYPE`, on ne met pas d’encodage → pas de bloc `lc_ctype` dans le DPB → la connexion reste en charset par défaut (NONE) ;
  - si l’utilisateur a mis `ISC_DPB_LC_CTYPE`, on essaie de trouver un `QStringConverter` correspondant, mais **on ne force rien** si ce n’est pas supporté.
- En **MODERN** ou **AUTO** :
  - si `ISC_DPB_LC_CTYPE` est vide, on force `UTF-8` (comportement Qt 6 standard),
  - on essaie de trouver un `QStringConverter` pour l’encodage, sinon fallback vers `UTF-8`.

On mémorise également un booléen temporaire `legacyNoLcCtypeForThisOpen` pour cette ouverture, que l’on stockera dans `qLegacyNoLcCtypeMap` une fois la connexion attachée.

#### Bloc de code utilisé

Dans `QFirebirdDriver::open(...)`, la partie encodage devient :

```cpp
// --- GESTION DU MODE D'ENCODAGE (REX_ENCODING_MODE) ---

// Valeur par défaut : pas de mode legacy brut
bool legacyNoLcCtype = false;

if (rexMode == RexEncodingMode::Legacy) {
    // Mode LEGACY :
    // - Si l'utilisateur n'a PAS mis ISC_DPB_LC_CTYPE, on laisse encString vide
    //   => aucun bloc isc_dpb_lc_ctype dans le DPB, connexion "brute" (NONE).
    // - Si l'utilisateur a mis ISC_DPB_LC_CTYPE, on le respecte et on tente
    //   de trouver un QStringConverter correspondant.
    if (encString.isEmpty()) {
        legacyNoLcCtype = true;
        qInfo("QFirebirdDriver: LEGACY mode without ISC_DPB_LC_CTYPE -> no lc_ctype in DPB.");
    } else {
        auto encodingOp = QStringConverter::encodingForName(encString.toLocal8Bit());
        if (encodingOp.has_value()) {
            d->encoding = encodingOp.value();
        } else {
            qWarning("QFirebirdDriver: unsupported encoding '%s' in LEGACY mode. "
                     "No QStringConverter encoding will be applied.",
                     qPrintable(encString));
            // On ne modifie pas d->encoding ici.
        }
    }
} else {
    // Mode MODERN ou AUTO : si aucun ISC_DPB_LC_CTYPE fourni, on force UTF-8
    if (encString.isEmpty()) {
        encString = u"UTF-8"_s;
    }

    auto encodingOp = QStringConverter::encodingForName(encString.toLocal8Bit());
    if (encodingOp.has_value()) {
        d->encoding = encodingOp.value();
    } else {
        qWarning("QFirebirdDriver: unsupported encoding '%s'. Falling back to UTF-8.",
                 qPrintable(encString));
        encString = u"UTF-8"_s;
        d->encoding = QStringConverter::Utf8;
    }
}

// On mémorise le flag legacyNoLcCtype globalement pour cette connexion (handle Firebird).
// À ce stade d->firebird vaut encore 0, on mettra à jour après isc_attach_database.
// On utilise une valeur temporaire : on stockera vraiment après attach.
bool legacyNoLcCtypeForThisOpen = legacyNoLcCtype;

QByteArray enc = encString.toLocal8Bit(); //"UTF8";
```

Et lors de la construction du DPB (`ba`), la partie `lc_ctype` devient conditionnelle :

```cpp
if (!enc.isEmpty()) {
    ba.append(char(isc_dpb_lc_ctype));
    ba.append(char(enc.length()));
    ba.append(enc.constData(), enc.length());
}
```

Après l’attachement de la base (`isc_attach_database`), on enregistre le flag dans `qLegacyNoLcCtypeMap` :

```cpp
isc_attach_database(d->status, 0, const_cast<char *>(ldb.toLocal8Bit().constData()),
                    &d->firebird, ba.size(), ba.constData());
if (d->isError(QT_TRANSLATE_NOOP("QFirebirdDriver", "Error opening database"),
               QSqlError::ConnectionError)) {
    setOpenError(true);
    return false;
}

// Maintenant d->firebird est valide : on enregistre le flag legacyNoLcCtype
if (qLegacyNoLcCtypeMap()) {
    (*qLegacyNoLcCtypeMap())[d->firebird] = legacyNoLcCtypeForThisOpen;
}

setOpen(true);
```

### 3.3. Décodage en Latin‑1 en mode LEGACY dans `gotoNext()`

`QFirebirdResult::gotoNext(...)` est responsable de la conversion des données brutes (`sqlvar.sqldata`) en `QVariant`/`QString`.

Pour les champs texte, le driver Qt 6 natif utilise toujours `QString::fromUtf8` :

```cpp
case SQL_VARYING:
    row[idx] = QString::fromUtf8(buf + sizeof(short), *(short*)buf);
    break;
...
case SQL_TEXT:
    row[idx] = QString::fromUtf8(buf, size);
    break;
```

En mode LEGACY sur une base `NONE`, ce n’est pas ce qu’on veut, car les données sont en 8 bits. On veut donc :

- pour la connexion en mode LEGACY **sans `lc_ctype`** (flag `legacyNoLcCtype == true` pour ce handle) :
  - décoder en **Latin‑1** (`QString::fromLatin1`);
- sinon :
  - garder le comportement normal (UTF‑8, `fromUtf8`).

On détermine le flag `legacyNoLcCtype` à partir de `d->firebird` dans `QFirebirdResultPrivate`, en consultant la map globale `qLegacyNoLcCtypeMap`.

Extrait de code utilisé dans `gotoNext(...)` :

```cpp
const char *buf = sqlvar.sqldata;
int size = sqlvar.sqllen;
Q_ASSERT(buf);
const auto sqltype = sqlvar.sqltype & ~1;
switch (sqltype) {
case SQL_VARYING: {
    // pascal strings - a short avec une longueur + les données
    const short len = *(short*)buf;
    const char *dataPtr = buf + sizeof(short);

    bool legacyNoLc = false;
    if (qLegacyNoLcCtypeMap()) {
        legacyNoLc = qLegacyNoLcCtypeMap()->value(d->firebird, false);
    }

    if (legacyNoLc) {
        // Mode legacy "brut" pour base NONE : on suppose des octets 8 bits (latin1-like)
        row[idx] = QString::fromLatin1(dataPtr, len);
    } else {
        // Comportement Qt 6 standard : UTF-8
        row[idx] = QString::fromUtf8(dataPtr, len);
    }
    break;
}
...
case SQL_TEXT: {
    const char *dataPtr = buf;
    const int len = size;

    bool legacyNoLc = false;
    if (qLegacyNoLcCtypeMap()) {
        legacyNoLc = qLegacyNoLcCtypeMap()->value(d->firebird, false);
    }

    if (legacyNoLc) {
        row[idx] = QString::fromLatin1(dataPtr, len);
    } else {
        row[idx] = QString::fromUtf8(dataPtr, len);
    }
    break;
}
```

Les autres cas (`SQL_INT64`, `SQL_TIMESTAMP`, etc.) restent inchangés.

---

## 4. Utilisation du driver

### 4.1. Mode LEGACY (recommandé pour ta base ancienne `charset NONE`)

Ce mode est pensé pour les bases historiques où :

- `RDB$CHARACTER_SET_NAME` est `NONE` ;
- les données texte sont en fait des octets 8 bits (latin1/Win‑1252 typiquement) ;
- tu veux retrouver un comportement proche de l’ancien driver Qt 5.15.

Dans ton code, au moment de configurer la base Qt :

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QFIREBIRD");

// ... host, dbName, user, password, etc.
db.setDatabaseName("chemin/vers/ma_base.fdb");
db.setUserName("SYSDBA");
db.setPassword("masterkey");

// IMPORTANT : mode legacy pour base NONE
db.setConnectOptions("REX_ENCODING_MODE=LEGACY");

if (!db.open()) {
    qDebug() << "Erreur ouverture base:" << db.lastError().text();
}
```

Comportement :

- Si tu ne spécifies pas `ISC_DPB_LC_CTYPE` :
  - aucun bloc `lc_ctype` n’est envoyé dans le DPB ;
  - la map globale marque la connexion comme `legacyNoLcCtype = true` ;
  - à la lecture, les champs `TEXT` / `VARCHAR` sont décodés avec `QString::fromLatin1(...)` ;
  - tes octets 8 bits sont convertis en `QString` comme en Latin‑1 (qui couvre la plupart des caractères accentués courants en français).

- Si tu mets **en plus** un `ISC_DPB_LC_CTYPE` explicite dans `connOpts`, le mode LEGACY :
  - respecte cette valeur, et tente de trouver un `QStringConverter` correspondant ;
  - n’est plus en mode « brut » (`legacyNoLcCtype = false`), et on garde `fromUtf8` (ou autre selon l’encodage choisi).

### 4.2. Mode MODERN (UTF‑8)

Pour une base moderne correctement configurée en `UTF8`, on utilisera :

```cpp
db.setConnectOptions("REX_ENCODING_MODE=MODERN");
```

ou simplement :

```cpp
db.setConnectOptions(QString()); // ou ne rien mettre
```

Comportement :

- Si aucun `ISC_DPB_LC_CTYPE` n’est fourni, le driver force `UTF-8` dans le DPB.
- Les champs texte sont lus avec `QString::fromUtf8`.
- C’est le chemin recommandé pour les nouvelles bases, où les colonnes sont en `UTF8` et les clients écrivent en UTF‑8.

### 4.3. Exemple de test dans l’appli

Pour vérifier visuellement le comportement, un petit code de debug :

```cpp
QVariant v = m_querySql.value(m_astModelNo);
QString m_astModelData = v.toString();
QByteArray rawData = v.toByteArray();

qDebug() << "---->Octets bruts avec OCTETS:" << rawData.toHex(' ');
qDebug() << "---->Texte Latin1:" << QString::fromLatin1(rawData);
qDebug() << "---->Texte direct:" << m_astModelData;
```

En mode LEGACY (sans `ISC_DPB_LC_CTYPE`) sur ta base `NONE`, pour `"Série 4000"` tu devrais obtenir quelque chose comme :

- Octets bruts : `53 e9 72 69 65 20 34 30 30 30` (ou autre 8 bits valide pour `é`),
- Texte direct : `Série 4000`.

---

## 5. Résumé

- Le problème venait du mélange :
  - base Firebird **charset NONE** avec données 8 bits,
  - et driver Qt 6 forçant **UTF‑8** (connexion + décodage).
- Nous avons introduit un paramètre `REX_ENCODING_MODE` dans `setConnectOptions()` :
  - `LEGACY` : pas de `lc_ctype` si absent, lecture des champs texte en **Latin‑1**.
  - `MODERN` (ou défaut) : `ISC_DPB_LC_CTYPE=UTF8`, lecture en **UTF‑8**.
- Une map globale associe chaque `isc_db_handle` à un flag `legacyNoLcCtype`, utilisé à la lecture pour choisir entre `fromLatin1` et `fromUtf8`.

En pratique, pour ta base Firebird ancienne en `NONE`, il suffit de :

```cpp
db.setConnectOptions("REX_ENCODING_MODE=LEGACY");
```

et de recompiler/installler le plugin Firebird Qt 6 modifié.  
Les chaînes accentuées comme `"Série 4000"` sont alors correctement récupérées.