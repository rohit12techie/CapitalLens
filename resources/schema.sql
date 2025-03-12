-- schema.sql

CREATE TABLE IF NOT EXISTS Users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_name TEXT UNIQUE NOT NULL
);

INSERT INTO Users (user_name) VALUES ('admin');

CREATE TABLE IF NOT EXISTS Years (
    year_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    year INTEGER NOT NULL,
    FOREIGN KEY(user_id) REFERENCES Users(user_id),
    UNIQUE(user_id, year)
);

CREATE TABLE IF NOT EXISTS Months (
    month_id INTEGER PRIMARY KEY AUTOINCREMENT,
    year_id INTEGER,
    month TEXT NOT NULL,
    FOREIGN KEY(year_id) REFERENCES Years(year_id),
    UNIQUE(year_id, month)
);

CREATE TABLE IF NOT EXISTS investments (
    entry_id INTEGER PRIMARY KEY AUTOINCREMENT,
    month_id INTEGER,
    type TEXT,
    amount REAL,
    comment TEXT,
    created_at TEXT,
    FOREIGN KEY(month_id) REFERENCES Months(month_id)
);

-- CREATE TABLE IF NOT EXISTS portfolio (
--     entry_id INTEGER PRIMARY KEY AUTOINCREMENT,
--     month_id INTEGER,
--     type TEXT,
--     amount REAL,
--     comment TEXT,
--     created_at TEXT,
--     FOREIGN KEY(month_id) REFERENCES Months(month_id)
-- );

-- CREATE TABLE IF NOT EXISTS cashin (
--     entry_id INTEGER PRIMARY KEY AUTOINCREMENT,
--     month_id INTEGER,
--     type TEXT,
--     amount REAL,
--     comment TEXT,
--     created_at TEXT,
--     FOREIGN KEY(month_id) REFERENCES Months(month_id)
-- );

-- CREATE TABLE IF NOT EXISTS cashout (
--     entry_id INTEGER PRIMARY KEY AUTOINCREMENT,
--     month_id INTEGER,
--     type TEXT,
--     amount REAL,
--     comment TEXT,
--     created_at TEXT,
--     FOREIGN KEY(month_id) REFERENCES Months(month_id)
-- );

-- CREATE TABLE IF NOT EXISTS investments (
--     id INTEGER PRIMARY KEY AUTOINCREMENT,
--     user TEXT NOT NULL,
--     month TEXT NOT NULL,
--     type TEXT NOT NULL,
--     amount REAL NOT NULL,
--     comment TEXT,
--     created_at DATETIME DEFAULT CURRENT_TIMESTAMP
-- );


CREATE TABLE IF NOT EXISTS portfolio (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user TEXT NOT NULL,
    month TEXT NOT NULL,
    type TEXT NOT NULL,
    amount REAL NOT NULL,
    comment TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS cashin (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user TEXT NOT NULL,
    month TEXT NOT NULL,
    type TEXT NOT NULL,
    amount REAL NOT NULL,
    comment TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS cashout (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user TEXT NOT NULL,
    month TEXT NOT NULL,
    type TEXT NOT NULL,
    amount REAL NOT NULL,
    comment TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);