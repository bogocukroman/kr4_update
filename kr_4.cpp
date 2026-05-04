#include <iostream>
#include <string>
#include <fstream>
#include <clocale>
#include <cstring>

using namespace std;

enum Comfort { LUX, JUNIOR_SUITE, STANDARD, ECONOMY };

struct RoomDetails {
    char options[100];
    bool seaView;
};

struct HotelRoom {
    char hotelName[50];
    int roomNumber;
    Comfort comfort;
    int capacity;
    double price;
    RoomDetails extra;
};


void manualSwap(HotelRoom& a, HotelRoom& b) {
    HotelRoom temp = a;
    a = b;
    b = temp;
}

void printRoom(const HotelRoom& r) {
    cout << "Отель: " << r.hotelName << " | №" << r.roomNumber
        << " | Цена: " << r.price << " | Опции: " << r.extra.options
        << (r.extra.seaView ? " [Море]" : "") << endl;
}

void displayFilteredRooms(HotelRoom* rooms, int size, string title) {
    cout << "\n--- " << title << " ---" << endl;
    if (size <= 0 || rooms == nullptr) {
        cout << "Список пуст." << endl;
        return;
    }
    for (int i = 0; i < size; i++) {
        printRoom(rooms[i]);
    }
}

void saveToBinary(const char* filename, HotelRoom* rooms, int size) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (fout.is_open()) {
        fout.write((char*)&size, sizeof(size));
        fout.write((char*)rooms, size * sizeof(HotelRoom));
        fout.close();
    }
}

HotelRoom* loadFromBinary(const char* filename, int& size) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) {
        size = 0;
        return nullptr;
    }
    fin.read((char*)&size, sizeof(size));
    HotelRoom* rooms = new HotelRoom[size];
    fin.read((char*)rooms, size * sizeof(HotelRoom));
    fin.close();
    return rooms;
}

void syncDataFromText(HotelRoom* rooms, int size) {
    ifstream txtFile("updates.txt");
    if (!txtFile.is_open()) return;
    int fileRoomNum;
    double fileNewPrice;
    while (txtFile >> fileRoomNum >> fileNewPrice) {
        for (int i = 0; i < size; i++) {
            if (rooms[i].roomNumber == fileRoomNum) {
                rooms[i].price = fileNewPrice;
            }
        }
    }
    txtFile.close();
}

void bubbleSort(HotelRoom* rooms, int size) {
    if (size <= 1) return;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            if (rooms[j].price > rooms[j + 1].price) {
                manualSwap(rooms[j], rooms[j + 1]); 
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    const char* binFile = "hotel_data.dat";

    int allCount = 0;
    HotelRoom* allRooms = loadFromBinary(binFile, allCount);

    if (allRooms == nullptr) {
        allCount = 19;
        allRooms = new HotelRoom[allCount]{
            {"Grand Plaza", 101, LUX, 2, 8000, {"WiFi, Bar", true}},
            {"Ocean Plaza", 102, STANDARD, 2, 2000, {"TV, AC", true}},
            {"Sky Plaza", 103, ECONOMY, 1, 1000, {"None", false}},
            {"Beach Resort", 104, LUX, 2, 9000, {"All inclusive", true}},
            {"City Plaza", 105, STANDARD, 1, 1500, {"WiFi", false}},
            {"Golden Plaza", 201, JUNIOR_SUITE, 2, 4500, {"TV, Balcony", true}},
            {"Mountain View", 202, ECONOMY, 2, 1300, {"Heater", false}},
            {"Royal Plaza", 203, LUX, 3, 12000, {"Jacuzzi, Gym", false}},
            {"Sea Breeze", 204, STANDARD, 2, 3200, {"AC, Terrace", true}},
            {"Central Hotel", 301, ECONOMY, 1, 900, {"None", false}},
            {"Emerald Plaza", 302, JUNIOR_SUITE, 2, 5500, {"WiFi, Coffee", true}},
            {"Sunset Plaza", 303, STANDARD, 2, 2800, {"TV, Sea-view", true}},
            {"Park Inn", 304, ECONOMY, 1, 1100, {"WiFi", false}},
            {"Marina Plaza", 401, LUX, 2, 8800, {"Bar, Pool", true}},
            {"Nordic Star", 402, STANDARD, 3, 3500, {"TV, AC", false}},
            {"Azure Plaza", 403, JUNIOR_SUITE, 2, 5100, {"Balcony", true}},
            {"Hostel 24", 501, ECONOMY, 4, 800, {"Kitchen", false}},
            {"Elite Plaza", 502, LUX, 2, 15000, {"Spa, Chef", true}},
            {"Green Valley", 503, STANDARD, 2, 2200, {"Garden", false}}
        };
    }

    syncDataFromText(allRooms, allCount);

    int searchId = 102;
    cout << "\n--- Поиск номера " << searchId << " ---" << endl;
    bool found = false;
    for (int i = 0; i < allCount; i++) {
        if (allRooms[i].roomNumber == searchId) {
            printRoom(allRooms[i]);
            found = true;
            break;
        }
    }
    if (!found) cout << "Номер не найден." << endl;

    int plazaCount = 0;
    for (int i = 0; i < allCount; i++) {
        if (string(allRooms[i].hotelName).find("Plaza") != string::npos) plazaCount++;
    }

    if (plazaCount > 0) {
        HotelRoom* plazaRooms = new HotelRoom[plazaCount];
        int idx = 0;
        for (int i = 0; i < allCount; i++) {
            if (string(allRooms[i].hotelName).find("Plaza") != string::npos) {
                plazaRooms[idx++] = allRooms[i];
            }
        }
        bubbleSort(plazaRooms, plazaCount);

        cout << "\n--- ТОП-3 дешевых в Plaza ---" << endl;
        for (int i = 0; i < 3 && i < plazaCount; i++) {
            cout << "№" << plazaRooms[i].roomNumber
                << " | Цена: " << plazaRooms[i].price
                << " | Опции: " << plazaRooms[i].extra.options << endl;
        }
        delete[] plazaRooms;
    }

    int seaCount = 0;
    for (int i = 0; i < allCount; i++) {
        if (allRooms[i].extra.seaView) seaCount++;
    }

    if (seaCount > 0) {
        HotelRoom* seaViewRooms = new HotelRoom[seaCount];
        int idx = 0;
        for (int i = 0; i < allCount; i++) {
            if (allRooms[i].extra.seaView) {
                seaViewRooms[idx++] = allRooms[i];
            }
        }
        displayFilteredRooms(seaViewRooms, seaCount, "Номера с видом на море");
        delete[] seaViewRooms;
    }

    saveToBinary(binFile, allRooms, allCount);
    delete[] allRooms;

    return 0;
}
