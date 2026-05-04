#include <iostream>
#include <string>
#include <fstream>
#include <clocale>

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


bool hasPlaza(const char* name) {
    const char* target = "Plaza";
    if (!name) return false;
    for (int i = 0; name[i] != '\0'; i++) {
        int j = 0;
        while (name[i + j] != '\0' && target[j] != '\0' && name[i + j] == target[j]) {
            j++;
        }
        if (target[j] == '\0') return true;
    }
    return false;
}

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

void displayFiltered(HotelRoom* hotelArray, int size, string title) {
    cout << "\n--- " << title << " ---" << endl;
    if (size <= 0 || hotelArray == nullptr) {
        cout << "Список пуст." << endl;
        return;
    }
    for (int i = 0; i < size; i++) {
        printRoom(hotelArray[i]);
    }
}

void saveToFile(const char* filename, HotelRoom* hotelArray, int size) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (fout.is_open()) {
        fout.write((char*)&size, sizeof(size));
        fout.write((char*)hotelArray, size * sizeof(HotelRoom));
        fout.close();
    }
}

HotelRoom* loadFromFile(const char* filename, int& size) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) {
        size = 0;
        return nullptr;
    }
    fin.read((char*)&size, sizeof(size));
    HotelRoom* hotelArray = new HotelRoom[size];
    fin.read((char*)hotelArray, size * sizeof(HotelRoom));
    fin.close();
    return hotelArray;
}

void updatePrices(HotelRoom* hotelArray, int size) {
    ifstream txtFile("updates.txt");
    if (!txtFile.is_open()) return;
    int fileRoomNum;
    double fileNewPrice;
    while (txtFile >> fileRoomNum >> fileNewPrice) {
        for (int i = 0; i < size; i++) {
            if (hotelArray[i].roomNumber == fileRoomNum) {
                hotelArray[i].price = fileNewPrice;
            }
        }
    }
    txtFile.close();
}

void sortByPrice(HotelRoom* hotelArray, int size) {
    if (size <= 1) return;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size - 1 - i; j++)
            if (hotelArray[j].price > hotelArray[j + 1].price)
                manualSwap(hotelArray[j], hotelArray[j + 1]);
}

int main() {
    setlocale(LC_ALL, "Russian");
    const char* binFile = "hotel_data.dat";

    int totalCount = 0;
    HotelRoom* allHotels = loadFromFile(binFile, totalCount);

    if (allHotels == nullptr) {
        totalCount = 19;
        allHotels = new HotelRoom[totalCount]{
            {"Grand Plaza", 101, LUX, 2, 8000, {"WiFi, Bar", true}},
            {"Ocean Plaza", 102, STANDARD, 2, 2000, {"TV, AC", true}},
            {"Sky Plaza", 103, ECONOMY, 1, 1000, {"WiFi", false}},
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

    updatePrices(allHotels, totalCount);


    int searchId = 102;
    bool found = false;
    for (int i = 0; i < totalCount; i++) {
        if (allHotels[i].roomNumber == searchId) {
            cout << "\n--- Найден номер " << searchId << " ---" << endl;
            printRoom(allHotels[i]);
            found = true; break;
        }
    }

    int plazaQty = 0;
    for (int i = 0; i < totalCount; i++) if (hasPlaza(allHotels[i].hotelName)) plazaQty++;

    if (plazaQty > 0) {
        HotelRoom* plazaList = new HotelRoom[plazaQty];
        int idx = 0;
        for (int i = 0; i < totalCount; i++) {
            if (hasPlaza(allHotels[i].hotelName)) plazaList[idx++] = allHotels[i];
        }
        sortByPrice(plazaList, plazaQty);
        displayFiltered(plazaList, (plazaQty > 3 ? 3 : plazaQty), "ТОП-3 дешевых в Plaza");
        delete[] plazaList;
    }


    int seaQty = 0;
    for (int i = 0; i < totalCount; i++) if (allHotels[i].extra.seaView) seaQty++;

    if (seaQty > 0) {
        HotelRoom* seaList = new HotelRoom[seaQty];
        int idx = 0;
        for (int i = 0; i < totalCount; i++) {
            if (allHotels[i].extra.seaView) seaList[idx++] = allHotels[i];
        }
        displayFiltered(seaList, seaQty, "Номера с видом на море");
        delete[] seaList;
    }

    saveToFile(binFile, allHotels, totalCount);
    delete[] allHotels;

    return 0;
}
