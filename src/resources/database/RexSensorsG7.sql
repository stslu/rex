-- MeasurementRanges database table
DROP TABLE IF EXISTS MeasurementRanges;

CREATE TABLE IF NOT EXISTS MeasurementRanges (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO MeasurementRanges (Name) VALUES
('MeasurementRange G7 1'),
('MeasurementRange G7 2'),
('MeasurementRange G7 3'),
('MeasurementRange G7 4'),
('MeasurementRange G7 5');

-- TheoricalPrecisions database table
DROP TABLE IF EXISTS TheoricalPrecisions;

CREATE TABLE IF NOT EXISTS TheoricalPrecisions (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO TheoricalPrecisions (Name) VALUES
('TheoricalPrecision G7 1'),
('TheoricalPrecision G7 2'),
('TheoricalPrecision G7 3'),
('TheoricalPrecision G7 4'),
('TheoricalPrecision G7 5');

-- Units database table
DROP TABLE IF EXISTS Units;

CREATE TABLE IF NOT EXISTS Units (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO Units (Name) VALUES
('Unit G7 1'),
('Unit G7 2'),
('Unit G7 3'),
('Unit G7 4'),
('Unit G7 5');

-- Experimentations database table
DROP TABLE IF EXISTS Experimentations;

CREATE TABLE IF NOT EXISTS Experimentations (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name VARCHAR(128) NOT NULL
);

INSERT INTO Experimentations (Name) VALUES
('Experimentation G7 1'),
('Experimentation G7 2'),
('Experimentation G7 3'),
('Experimentation G7 4'),
('Experimentation G7 5');

-- Sensors database table
DROP TABLE IF EXISTS Sensors;

CREATE TABLE IF NOT EXISTS Sensors (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Constructor_Id INTEGER NOT NULL,
    Model_Id INTEGER NOT NULL,
    Technology_Id INTEGER NOT NULL,
    PhysicalMeasurement_Id INTEGER NOT NULL,
    OutputSignal_Id INTEGER NOT NULL,
    MeasurementRange_Id INTEGER NOT NULL,
    TheoricalPrecision_Id INTEGER NOT NULL,
    Unit_Id INTEGER NOT NULL,
    Experimentation_Id INTEGER NOT NULL,    
    Name VARCHAR(128) NOT NULL,
    StartDate VARCHAR(128) NOT NULL,
    EndDate VARCHAR(128) DEFAULT '0000-00-00',   
    Info VARCHAR(128) NOT NULL,
    FOREIGN KEY (Constructor_Id) REFERENCES Constructors(Id),
    FOREIGN KEY (Model_Id) REFERENCES Models(Id),
    FOREIGN KEY (Technology_Id) REFERENCES Technologies(Id),
    FOREIGN KEY (PhysicalMeasurement_Id) REFERENCES PhysicalMeasurements(Id),
    FOREIGN KEY (OutputSignal_Id) REFERENCES OutputSignals(Id),
    FOREIGN KEY (MeasurementRange_Id) REFERENCES MeasurementRanges(Id),
    FOREIGN KEY (TheoricalPrecision_Id) REFERENCES TheoricalPrecisions(Id),
    FOREIGN KEY (Unit_Id) REFERENCES Units(Id),
    FOREIGN KEY (Experimentation_Id) REFERENCES Experimentations(Id)
);

INSERT INTO Sensors (Constructor_Id, Model_Id, Technology_Id, PhysicalMeasurement_Id, OutputSignal_Id, MeasurementRange_Id, TheoricalPrecision_Id, Unit_Id, Experimentation_Id, Name, StartDate, Info) VALUES
(1, 1, 1, 1, 1, 1, 1, 1, 1, 'Sensor 1', '2001-01-10', 'Info 1'),
(1, 2, 1, 1, 1, 1, 1, 1, 1, 'Sensor 2', '2002-01-10', 'Info 2'),
(1, 1, 3, 1, 1, 1, 1, 1, 1, 'Sensor 3', '2003-01-10', 'Info 3'),
(1, 1, 1, 4, 1, 1, 1, 1, 1, 'Sensor 4', '2004-01-10', 'Info 4'),
(1, 1, 1, 1, 5, 1, 1, 1, 1, 'Sensor 5', '2005-01-10', 'Info 5'),
(1, 1, 1, 1, 1, 1, 1, 1, 1, 'Sensor 6', '2006-01-10', 'Info 6'),
(1, 1, 1, 1, 1, 2, 1, 1, 1, 'Sensor 7', '2007-01-10', 'Info 7'),
(1, 1, 1, 1, 1, 1, 3, 1, 1, 'Sensor 8', '2008-01-10', 'Info 8'),
(1, 1, 1, 1, 1, 1, 1, 4, 1, 'Sensor 9', '2009-01-10', 'Info 9');

