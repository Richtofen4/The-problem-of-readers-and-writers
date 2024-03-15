#define NOMINMAX

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <random>
#include <chrono>
#include <Windows.h>

using namespace std;

// Global variables
mutex mtx;
condition_variable cv;
int activeReaders = 0, activeWriters = 0;
int waitingReaders = 0, waitingWriters = 0;
int totalReaders, totalWriters;
bool canReadersEnter = false;
default_random_engine rng;
int help;

// Function declarations
void reader(int id);
void writer(int id);
void enterReading(int id);
void exitReading(int id);
void enterWriting(int id);
void exitWriting(int id);
void displayStatus();

int main() {
    // Keystroke detection configuration
    thread watekSprawdzajacyKlawisze([]() {
        while (true) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                exit(0);
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        });
    watekSprawdzajacyKlawisze.detach();

    setlocale(0, "");
    cout << "Enter the number of readers: "; //Entering the number of readers
    cin >> totalReaders;

    if (totalReaders < 1) {
        cout << "The data provided is not a positive integer. The number of readers will be 10.\n";
        totalReaders = 10;
    }
    cin.clear();
    cin.ignore(std::numeric_limits < std::streamsize >::max(), '\n');


    cout << "Enter the number of writers: "; //Entering the number of writers
    cin >> totalWriters;

    if (totalWriters < 1) {
        cout << "The data provided is not a positive integer. The number of writers will be 5.\n";
        totalWriters = 5;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits < std::streamsize >::max(), '\n');


    vector<thread> threads;
    srand(time(NULL));
    rng.seed(chrono::system_clock::now().time_since_epoch().count());
    // Creating reader and writer threads
    for (int i = 0; i < totalReaders; ++i) {
        threads.push_back(thread(reader, i));
    }
    for (int i = 0; i < totalWriters; ++i) {
        threads.push_back(thread(writer, i));
    }

    // Creating a separate thread for status display
    thread statusThread(displayStatus);

    // Joining reader and writer threads
    for (auto& t : threads) {
        t.join();
    }

    // Joining a status thread
    statusThread.join();

    return 0;
}

void displayStatus() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(1)); // Update every second

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        // Clearing the console screen
        system("CLS");

        // Lock and view updated information
        unique_lock<mutex> lk(mtx);
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "Number of total readers: " << "\t\t" << totalReaders
            << "\nNumber of all writers: " << "\t\t\t" << totalWriters;
        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
        cout << "\nNumber of readers in the reading room: " << "\t" << activeReaders
            << "\nNumber of writers in the reading room: " << "\t" << activeWriters;
        SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
        cout << "\nNumber of waiting readers: " << "\t\t" << waitingReaders
            << "\nNumber of waiting writers: " << "\t\t" << waitingWriters << '\n';
        SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY);
        cout << "\nPress space to end the program" << endl;
        lk.unlock();
    }
}


void reader(int id) {
    while (true) {
        std::uniform_int_distribution<int> dist(1, 5); //Random delay generation
        int delay = dist(rng);
        std::this_thread::sleep_for(std::chrono::seconds(delay)); //Putting the thread to sleep

        enterReading(id);
        // Reading simulation
        std::this_thread::sleep_for(std::chrono::seconds(1));
        exitReading(id);
    }
}


void writer(int id) {
    while (true) {
        this_thread::sleep_for(chrono::seconds((rand() % 10) + 1)); // simulating inactive time
        enterWriting(id);

        this_thread::sleep_for(chrono::seconds((rand() % 5) + 1)); // Simulating writing time

        exitWriting(id);
    }
}

void enterReading(int id) {
    unique_lock<mutex> lk(mtx);
    waitingReaders++;
    //Checking for the condition that there are no active writers and readers who can come in or there are no waiting writers
    cv.wait(lk, [] {return activeWriters == 0 && (canReadersEnter || waitingWriters == 0); });
    help--;
    //Stopping readers from entering the reading room
    if (help == 0)
        canReadersEnter = false;
    waitingReaders--;
    activeReaders++;
    lk.unlock(); //Unlocking the mutex
}

void exitReading(int id) {
    unique_lock<mutex> lk(mtx); //Mutex lock
    activeReaders--;
    // If there are no active readers and there are waiting writers, block access for additional readers and notify one writer's thread
    if (activeReaders == 0 && waitingWriters > 0) {
        canReadersEnter = false;
        cv.notify_one();
    }
    lk.unlock(); //Unlocking the mutex
}

void enterWriting(int id) {
    unique_lock<mutex> lk(mtx); //Mutex lock
    waitingWriters++;
    // Waiting for the condition that there are no active readers and writers and readers cannot enter
    cv.wait(lk, [] {return activeReaders == 0 && activeWriters == 0 && !canReadersEnter; });
    waitingWriters--;
    activeWriters++;
    lk.unlock(); //Unlocking the mutex
}

void exitWriting(int id) {
    unique_lock<mutex> lk(mtx); //Mutex lock
    activeWriters--;
    // If there are waiting readers, let them come in
    if (waitingReaders > 0) {
        canReadersEnter = true;
        help = waitingReaders;
        cv.notify_all(); // Notification of all pending threads
    }
    else
        cv.notify_one(); // If there are no waiting readers, notify one thread
    lk.unlock(); //Unlocking the mutex
}