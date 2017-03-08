-- Constructors constructors table
DROP TABLE IF EXISTS Constructors;

CREATE TABLE IF NOT EXISTS Constructors (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO Constructors (Name) VALUES
('Constructor G6 1'),
('Constructor G6 2'),
('Constructor G6 3'),
('Constructor G6 4'),
('Constructor G6 5');

-- Models database table
DROP TABLE IF EXISTS Models;

CREATE TABLE IF NOT EXISTS Models (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO Models (Name) VALUES
('Model G6 1'),
('Model G6 2'),
('Model G6 3'),
('Model G6 4'),
('Model G6 5');

-- Technologies database table
DROP TABLE IF EXISTS Technologies;

CREATE TABLE IF NOT EXISTS Technologies (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO Technologies (Name) VALUES
('Technology G6 1'),
('Technology G6 2'),
('Technology G6 3'),
('Technology G6 4'),
('Technology G6 5');

-- PhysicalMeasurements database table
DROP TABLE IF EXISTS PhysicalMeasurements;

CREATE TABLE IF NOT EXISTS PhysicalMeasurements (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO PhysicalMeasurements (Name) VALUES
('PhysicalMeasurement G6 1'),
('PhysicalMeasurement G6 2'),
('PhysicalMeasurement G6 3'),
('PhysicalMeasurement G6 4'),
('PhysicalMeasurement G6 5');

-- OutputSignals database table
DROP TABLE IF EXISTS OutputSignals;

CREATE TABLE IF NOT EXISTS OutputSignals (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO OutputSignals (Name) VALUES
('OutputSignal G6 1'),
('OutputSignal G6 2'),
('OutputSignal G6 3'),
('OutputSignal G6 4'),
('OutputSignal G6 5');