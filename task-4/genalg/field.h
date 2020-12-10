#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#define SIZE 50

using Configuration = std::vector<std::vector<int>>;

class Field {
    Configuration field;
public:
    Field();

    Field(const Configuration &x);

    int NumFilled() const;

    void Step();

    void PrintField() const;

    bool operator!=(Field &x);

    Configuration FieldConf() const;

    void PrintFormatField(std::ofstream &ofm) const;

    void PrintFormatField() const;
};


Field::Field() {
    for (int i = 0; i < SIZE; i++) {
        std::vector<int> line(SIZE);
        for (int j = 0; j < SIZE; j++) {
            line[j] = rand() % 2;
        }
        field.push_back(line);
    }
}

Field::Field(const Configuration &x) : field(x) {}

int Field::NumFilled() const {
    int num = 0;
    for (const auto &line : field) {
        num += std::count_if(line.begin(), line.end(), [](int i) { return i == 1; });
    }
    return num;
}

void Field::Step() {
    Configuration tmp = field;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int count = 0;

            count += (i && field[i - 1][j]) ? 1 : 0;
            count += ((j + 1) < SIZE && field[i][j + 1]) ? 1 : 0;
            count += (j && field[i][j - 1]) ? 1 : 0;
            count += (i && j && field[i - 1][j - 1]) ? 1 : 0;
            count += ((i + 1) < SIZE && field[i + 1][j]) ? 1 : 0;
            count += ((i + 1) < SIZE && (j + 1) < SIZE && field[i + 1][j + 1]) ? 1 : 0;
            count += ((i + 1) < SIZE && j && field[i + 1][j - 1]) ? 1 : 0;
            count += ((j + 1) < SIZE && i && field[i - 1][j + 1]) ? 1 : 0;

            if (field[i][j]) {
                tmp[i][j] = (count < 2 || count > 3) ? 0 : 1;
            } else {
                tmp[i][j] = (count != 3) ? 0 : 1;
            }
        }
    }
    field = tmp;
}

void Field::PrintField() const {
    for (const auto &line : field) {
        for (const auto &elem : line) {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }
}

bool Field::operator!= (Field &x) {
    bool res = true;
    for (auto i = 0; i < SIZE; i++) {
        res &= (x.FieldConf()[i] == field[i]);
    }
    return !res;
}

Configuration Field::FieldConf() const {
    return field;
}

void Field::PrintFormatField(std::ofstream &fout) const {
    for (const auto &line : field) {
        for (const auto &elem : line) {
            fout << ((elem) ? "X" : "-") << " ";
        }
        fout << "\n";
    }
}

void Field::PrintFormatField() const {
    std::cout << std::endl;
    for (const auto &line : field) {
        std::cout << "     ";
        for (const auto &elem : line) {
            std::cout << ((elem) ? "X" : "-") << " ";
        }
        std::cout << "\n";
    }
}