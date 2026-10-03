# Priority Based Schedule and Conflict Resolver

A C++ based schedule management system that helps manage weekly activities and resolve schedule conflicts.

## Features

- Add, show, and delete schedules
- Detect overlapping activities
- Resolve conflicts based on activity type and priority
- Recommend available free slots for rescheduling
- Save and load schedule data using a local text file

## How It Works

The user adds an activity by entering its title, day, time, activity type, and priority.

The system checks whether the new activity conflicts with an existing schedule. If a conflict occurs, the system applies priority-based rules to determine which activity is kept. If an activity needs to be rescheduled, the system recommends available free slots.

## Project Files

- `Source Code_Group 3.cpp` - Main C++ source code
- `Flowchart_Group 3.png` - System flowchart
- `Presentation_Group 3.pdf` - Project presentation
