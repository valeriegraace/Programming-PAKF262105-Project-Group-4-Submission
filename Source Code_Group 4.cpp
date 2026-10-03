#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

const int DAYS = 7;
const int DAY_START = 7 * 60;
const int DAY_END = 22 * 60;

struct Activity {
    string name;
    int day;
    int start;
    int end;
    int priority;
    int type; // 1 = Fixed, 2 = Flexible
};

vector<Activity> schedule;

void showTime(int minutes) {
    int hour = minutes / 60;
    int minute = minutes % 60;

    if (hour < 10) cout << "0";
    cout << hour << ":";

    if (minute < 10) cout << "0";
    cout << minute;
}

int inputTime(string message) {
    int hour, minute;

    while (true) {
        cout << message << " (HH MM): ";
        cin >> hour >> minute;

        if (hour >= 0 && hour <= 23 &&
            minute >= 0 && minute <= 59) {
            return hour * 60 + minute;
        }

        cout << "Invalid time.\n";
    }
}

bool isOverlap(Activity a, Activity b) {
    if (a.day != b.day)
        return false;

    if (a.start < b.end && b.start < a.end)
        return true;

    return false;
}

int getOverlap(Activity a, Activity b) {
    int start = a.start;
    int end = a.end;

    if (b.start > start) start = b.start;
    if (b.end < end) end = b.end;

    return end - start;
}

bool isFree(int day, int start, int end, int ignoreIndex) {
    for (int i = 0; i < (int)schedule.size(); i++) {
        if (i == ignoreIndex)
            continue;

        if (schedule[i].day == day &&
            start < schedule[i].end &&
            schedule[i].start < end) {
            return false;
        }
    }

    return true;
}

bool isFree(int day, int start, int end, int ignoreIndex, Activity blockedActivity) {
    if (!isFree(day, start, end, ignoreIndex))
        return false;

    if (blockedActivity.day == day &&
        start < blockedActivity.end &&
        blockedActivity.start < end) {
        return false;
    }

    return true;
}

void addActivity(Activity newActivity) {
    schedule.push_back(newActivity);

    sort(schedule.begin(), schedule.end(),
         [](Activity a, Activity b) {
             if (a.day != b.day)
                 return a.day < b.day;
             return a.start < b.start;
         });

    cout << "\nActivity added successfully.\n";
}

void removeActivity(int index) {
    schedule.erase(schedule.begin() + index);
}

void showSchedule();

void deleteSchedule() {
    if (schedule.empty()) {
        cout << "\nSchedule is empty.\n";
        return;
    }

    showSchedule();

    int choice;
    cout << "\nEnter schedule number to delete (0 to cancel): ";
    cin >> choice;

    if (choice == 0)
        return;

    if (choice < 1 || choice > (int)schedule.size()) {
        cout << "Invalid choice.\n";
        return;
    }

    removeActivity(choice - 1);
    cout << "Schedule deleted.\n";
}

void showSchedule() {
    if (schedule.empty()) {
        cout << "\nSchedule is empty.\n";
        return;
    }

    string dayName[DAYS] = {
        "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday", "Sunday"
    };

    cout << "\n========== SCHEDULE ==========\n";

    for (int i = 0; i < (int)schedule.size(); i++) {
        cout << i + 1 << ". " << schedule[i].name << "\n";
        cout << "   Day      : " << dayName[schedule[i].day] << "\n";
        cout << "   Time     : ";
        showTime(schedule[i].start);
        cout << " - ";
        showTime(schedule[i].end) ;
        cout << "\n";
        cout << "   Type     : ";
        if (schedule[i].type == 1)
            cout << "Fixed\n";
        else
            cout << "Flexible\n";

        if (schedule[i].type == 2)
            cout << "   Priority : " << schedule[i].priority << "\n";
    }

    cout << "==============================\n";
}

bool manualFreeSlot(Activity &activity, int ignoreIndex, Activity blockedActivity) {
    int duration = activity.end - activity.start;

    cout << "\nManual reschedule\n";
    cout << "Enter day (1=Monday ... 7=Sunday): ";
    cin >> activity.day;

    if (activity.day < 1 || activity.day > 7) {
        cout << "Invalid day.\n";
        return false;
    }

    activity.day--;

    int newStart = inputTime("Enter new start time");
    int newEnd = newStart + duration;

    if (newEnd > 24 * 60) {
        cout << "The activity goes past midnight. Not allowed.\n";
        return false;
    }

    if (newStart < DAY_START || newEnd > DAY_END) {
        cout << "The new time must be between 07:00 and 22:00.\n";
        return false;
    }

    if (!isFree(activity.day, newStart, newEnd, ignoreIndex, blockedActivity)) {
        cout << "That time conflicts with another schedule.\n";
        return false;
    }

    activity.start = newStart;
    activity.end = newEnd;

    return true;
}

bool suggestFreeSlots(Activity &activity, int ignoreIndex, Activity blockedActivity) {
    int duration = activity.end - activity.start;
    int slots[5];
    int slotDay[5];
    int slotCount = 0;

    for (int day = 0; day < DAYS && slotCount < 5; day++) {
        for (int time = DAY_START;
             time + duration <= DAY_END && slotCount < 5;
             time++) {

            if (isFree(day, time, time + duration, ignoreIndex, blockedActivity)) {
                slots[slotCount] = time;
                slotDay[slotCount] = day;
                slotCount++;

                time = time + duration - 1;
            }
        }
    }

    if (slotCount == 0) {
        cout << "\nNo recommended free slot was found.\n";
    } else {
        string dayName[DAYS] = {
            "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturday", "Sunday"
        };

        cout << "\nRecommended free slots:\n";

        for (int i = 0; i < slotCount; i++) {
            cout << i + 1 << ". " << dayName[slotDay[i]] << " ";
            showTime(slots[i]);
            cout << " - ";
            showTime(slots[i] + duration);
            cout << "\n";
        }
    }

    cout << "6. Manual input\n";
    cout << "0. Cancel\n";

    int choice;
    cout << "Choose: ";
    cin >> choice;

    if (choice == 0) {
        cout << "Reschedule cancelled.\n";
        return false;
    }

    if (choice == 6) {
        if (manualFreeSlot(activity, ignoreIndex, blockedActivity)) {
            cout << "Activity manually rescheduled.\n";
            return true;
        }

        cout << "Manual reschedule failed.\n";
        return false;
    }

    if (choice < 1 || choice > slotCount) {
        cout << "Invalid choice.\n";
        return false;
    }

    activity.day = slotDay[choice - 1];
    activity.start = slots[choice - 1];
    activity.end = activity.start + duration;

    return true;
}

bool resolveMajorConflict(Activity &newActivity, int conflictIndex) {
    Activity oldActivity = schedule[conflictIndex];
    // New Fixed vs old Flexible
    if (newActivity.type == 1 && oldActivity.type == 2) {
        cout << "\nFixed activity stays.\n";
        cout << "Flexible activity \"" << oldActivity.name
             << "\" must be rescheduled.\n";

        if (suggestFreeSlots(oldActivity, conflictIndex, newActivity)) {
            removeActivity(conflictIndex);
            addActivity(oldActivity);
            return true;
        }

        return false;
    }

    // New Flexible vs old Fixed
    if (newActivity.type == 2 && oldActivity.type == 1) {
        cout << "\nExisting Fixed activity stays.\n";
        cout << "New Flexible activity must be rescheduled.\n";

        if (suggestFreeSlots(newActivity, -1, oldActivity))
            addActivity(newActivity);

        return false;
    }

    // Flexible vs Flexible
    if (newActivity.type == 2 && oldActivity.type == 2) {
        if (newActivity.priority > oldActivity.priority) {
            cout << "\nNew activity has higher priority.\n";
            cout << "Existing activity will be rescheduled.\n";

            if (suggestFreeSlots(oldActivity, conflictIndex, newActivity)) {
                removeActivity(conflictIndex);
                addActivity(oldActivity);
                return true;
            }

            return false;
        }

        if (newActivity.priority < oldActivity.priority) {
            cout << "\nExisting activity has higher priority.\n";
            cout << "New activity will be rescheduled.\n";

            if (suggestFreeSlots(newActivity, -1, oldActivity))
                addActivity(newActivity);

            return false;
        }

        cout << "\nBoth activities have the same priority.\n";
        cout << "1. Keep new activity\n";
        cout << "2. Keep existing activity\n";
        cout << "0. Cancel\n";

        int choice;
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1) {
            if (suggestFreeSlots(oldActivity, conflictIndex, newActivity)) {
                removeActivity(conflictIndex);
                addActivity(oldActivity);
                return true;
            }
            return false;
        }

        if (choice == 2) {
            if (suggestFreeSlots(newActivity, -1, oldActivity))
                addActivity(newActivity);
            return false;
        }

        cout << "New activity cancelled.\n";
        return false;
    }

    return false;
}

void processActivity(Activity newActivity) {
    int i = 0;

    while (i < (int)schedule.size()) {
        if (isOverlap(newActivity, schedule[i])) {
            int overlap = getOverlap(newActivity, schedule[i]);

            cout << "\n========== CONFLICT ==========\n";
            cout << "New activity      : " << newActivity.name << "\n";
            cout << "Existing activity : " << schedule[i].name << "\n";
            cout << "Overlap           : " << overlap << " minutes\n";

            // Fixed vs Fixed: warning only
            if (newActivity.type == 1 &&
                schedule[i].type == 1) {
                cout << "[WARNING] Both activities are Fixed.\n";
                cout << "Please check the schedule again. "
                     << "No activity was changed.\n";
                i++;
                continue;
            }

            // Minor conflict
            if (overlap <= 10) {
                cout << "\nMinor conflict (10 minutes or less).\n";
                cout << "1. Ignore conflict\n";
                cout << "2. Reschedule new activity\n";
                cout << "0. Cancel\n";

                int choice;
                cout << "Choose: ";
                cin >> choice;

                if (choice == 1) {
                    i++;
                    continue;
                }

                if (choice == 2) {
                    if (suggestFreeSlots(newActivity, -1, schedule[i]))
                        addActivity(newActivity);
                    return;
                }

                cout << "New activity cancelled.\n";
                return;
            }

            // Major conflict
            bool keepNew = resolveMajorConflict(newActivity, i);

            if (!keepNew)
                return;

            // Existing conflicting activity was removed.
            continue;
        }

        i++;
    }

    addActivity(newActivity);
}

Activity inputActivity() {
    Activity activity;

    cin.ignore();

    cout << "\nEnter activity name: ";
    getline(cin, activity.name);

    cout << "Enter day (1=Monday ... 7=Sunday): ";
    cin >> activity.day;

    while (activity.day < 1 || activity.day > 7) {
        cout << "Invalid day. Enter again: ";
        cin >> activity.day;
    }

    activity.day--;

    activity.start = inputTime("Enter start time");
    activity.end = inputTime("Enter end time");

    while (activity.start >= activity.end) {
        cout << "Start time must be earlier than end time.\n";
        activity.start = inputTime("Enter start time");
        activity.end = inputTime("Enter end time");
    }

    cout << "Select type:\n";
    cout << "1. Fixed\n";
    cout << "2. Flexible\n";
    cout << "Choose: ";
    cin >> activity.type;

    while (activity.type != 1 && activity.type != 2) {
        cout << "Invalid choice. Enter 1 or 2: ";
        cin >> activity.type;
    }

    if (activity.type == 1) {
        activity.priority = 5;
    } else {
        cout << "Enter priority (1-5): ";
        cin >> activity.priority;

        while (activity.priority < 1 || activity.priority > 5) {
            cout << "Priority must be 1-5: ";
            cin >> activity.priority;
        }
    }

    return activity;
}

void saveToFile() {
    ofstream file("schedule.txt");

    if (!file) {
        cout << "\nFailed to save schedule.\n";
        return;
    }

    for (int i = 0; i < (int)schedule.size(); i++) {
        file << schedule[i].name << "|"
             << schedule[i].day << "|"
             << schedule[i].start << "|"
             << schedule[i].end << "|"
             << schedule[i].priority << "|"
             << schedule[i].type << "\n";
    }

    file.close();
}

void loadFromFile() {
    ifstream file("schedule.txt");

    if (!file) {
        return;
    }

    schedule.clear();

    string line;
    while (getline(file, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);

        if (p1 == string::npos || p2 == string::npos ||
            p3 == string::npos || p4 == string::npos ||
            p5 == string::npos) {
            continue;
        }

        Activity activity;
        activity.name = line.substr(0, p1);
        activity.day = stoi(line.substr(p1 + 1, p2 - p1 - 1));
        activity.start = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        activity.end = stoi(line.substr(p3 + 1, p4 - p3 - 1));
        activity.priority = stoi(line.substr(p4 + 1, p5 - p4 - 1));
        activity.type = stoi(line.substr(p5 + 1));

        if (activity.day >= 0 && activity.day < DAYS &&
            activity.start >= DAY_START &&
            activity.end <= DAY_END &&
            activity.start < activity.end &&
            (activity.type == 1 || activity.type == 2) &&
            activity.priority >= 1 && activity.priority <= 5) {
            schedule.push_back(activity);
        }
    }

    file.close();

    sort(schedule.begin(), schedule.end(),
         [](Activity a, Activity b) {
             if (a.day != b.day)
                 return a.day < b.day;
             return a.start < b.start;
         });
}

int main() {
    int menu;

    loadFromFile();

    do {
        cout << "\n========== CLASS SCHEDULE MANAGER ==========\n";
        cout << "1. Add Schedule\n";
        cout << "2. Show Schedule\n";
        cout << "3. Delete Schedule\n";
        cout << "4. Exit\n";
        cout << "Choose: ";
        cin >> menu;

        if (menu == 1) {
            Activity newActivity = inputActivity();
            processActivity(newActivity);
            saveToFile();

        } else if (menu == 2) {
            showSchedule();

        } else if (menu == 3) {
            deleteSchedule();
            saveToFile();

        } else if (menu == 4) {
            cout << "\nProgram ended.\n";

        } else {
            cout << "\nInvalid menu choice.\n";
        }

    } while (menu != 4);

    return 0;
}
