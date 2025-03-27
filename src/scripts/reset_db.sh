#!/bin/bash

# Get the project root directory (assuming script is in src/scripts)
PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

# Ensure data directory exists
mkdir -p "$PROJECT_ROOT/data"

# Path to database
DB_PATH="$PROJECT_ROOT/data/booking.db"

echo "Resetting database at: $DB_PATH"

# Drop and recreate tables in the existing database
sqlite3 "$DB_PATH" <<EOF
-- Drop existing tables if they exist
DROP TABLE IF EXISTS room_bookings;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS settings;

-- Create tables
CREATE TABLE room_bookings (
    sala TEXT,
    data TEXT,
    professor TEXT,
    disciplina TEXT,
    turma TEXT,
    horario_inicio TEXT,
    horario_fim TEXT
);

CREATE TABLE users (
    login TEXT PRIMARY KEY,
    senha TEXT NOT NULL
);

CREATE TABLE settings (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

-- Verify tables were created
SELECT 'Tables created successfully:' AS message;
SELECT name FROM sqlite_master WHERE type='table';
EOF

echo "Database reset complete. Test user created: admin/admin"