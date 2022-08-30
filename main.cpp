

/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * Parking lot program.
 * File:   main.cpp
 * Author: C Michael Sundius
 * Author: msundius@sundius.com
 *
 * Created on July 6, 2022, 7:47 PM
 * Copyright (c) 2022, C Michael Sundius, All rights reserved.
 */

#include <functional>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <deque>

using namespace std;

#define REGULAR     "REGULAR"
#define HANDICAPPED "HANDICAPPED"
#define ELECTRIC    "ELECTRIC"
#define COMPACT     "COMPACT"
#define ANY         "ANY"
#define DOESNT_RUN  "DOESNT RUN"

#define T_REGULAR     4
#define T_HANDICAPPED 3
#define T_ELECTRIC    2
#define T_COMPACT     1
#define T_ANY         0
#define T_DOESNT_RUN  -1

#define FAIL -1
#define SUCCESS 0

std::string car_names[] = {
    ANY,
    COMPACT,
    ELECTRIC,
    HANDICAPPED,
    REGULAR
};

int dmv_current_vin = 1;

bool vin_match(const Car car_p, int id) {
    if (car_p.vin == id)
        return true;
    else
        return false;
}

class Car {
public:
    Car(int typ);
    Car(int typ, int id);
    bool runs(){return !(car_type == T_DOESNT_RUN);}
    std::string getCarName() {
        if ((car_type == T_DOESNT_RUN))
            return DOESNT_RUN;
        return car_names[car_type];
    }
    void report_car() { cout << "car: " << this->getCarName() << ":" << vin << endl;}
    int car_type;
    int vin;
    
};

Car::Car(int typ) {
    if ((typ <= T_ANY)|| (typ > T_REGULAR)) car_type = T_DOESNT_RUN;
    else car_type = typ;
    if (car_type != T_DOESNT_RUN)
        cout << "new car: type " << typ << endl;
    vin = dmv_current_vin++;
}

Car::Car(int typ, int id) {
    if ((typ <= T_ANY)|| (typ > T_REGULAR)) car_type = T_DOESNT_RUN;
    else car_type = typ;
    if (car_type != T_DOESNT_RUN)
        cout << "new car: type " << typ << endl;
    vin = id;
}

class LotSection {
public:
    LotSection(int max) { capacity = max; }
    int get_capacity() { return capacity;}
    int park_car(Car *car);
    Car * unpark_car(int id);
    int get_nfilled() { return filled;}
    int get_navailable(){return capacity - filled;}
    void report_state();
    string type_str = ANY;
    int car_type = T_ANY;
    deque<Car *> slots;
    
private:
    int capacity;
    int filled;
};

void LotSection::report_state()
{
    cout << "section name:" << type_str << endl;
    cout << "capacity:" << capacity << endl;
    cout << "available:" << get_navailable() << endl;
}

Car * LotSection::unpark_car(int id) {
    deque<Car *>::iterator it;
    it = std::find_if(slots.begin(), slots.end(), [id](Car const *car_p)->bool{ return (car_p->vin == id)?true:false;});

    
    if (it != slots.end()) {
        Car *car_p = *it;
        cout << "we found your " << car_p->getCarName() << " car, id " << car_p->vin << " " << id << endl;
        slots.erase(it);
        return car_p;
    }
    return NULL;
}

int LotSection::park_car(Car *car) {
    cout << "lets park! " << car_type << car->car_type << endl;
     if (car->car_type != car_type &&
          car_type != T_ANY) {
         cout << "you can't park here!!!\n" << endl;
         return FAIL;
     }
     if(filled >= capacity) {
         cout << "lot is full\n" << endl;
         return FAIL;
     }
     filled++;
     slots.push_front(car);
     return SUCCESS;
}

class RSection:public LotSection {
public:
    RSection(int max):LotSection(max) { type_str = REGULAR; car_type = T_REGULAR;}

};

class HSection:public LotSection {
public:
    HSection(int max):LotSection(max) { type_str = HANDICAPPED; car_type = T_HANDICAPPED;}
};

class ESection:public LotSection {
public:
    ESection(int max):LotSection(max) { type_str = ELECTRIC; car_type = T_ELECTRIC;}
};

class CSection:public LotSection {
public:
    CSection(int max):LotSection(max) { type_str = COMPACT; car_type = T_COMPACT;}
};

class ParkingLot {
public:
    ParkingLot();
    void report_lot_state();
    void park(Car *car);
    Car * unpark(int id);
    Car * unpark(int section_id, int id);

private:
    HSection *hc_section;
    RSection *reg_section;
    ESection *el_section;
    CSection *comp_section;
    LotSection *lookup_section(int typ);
};

ParkingLot::ParkingLot() {
    hc_section = new HSection(25);
    reg_section= new RSection(25);
    el_section = new ESection(25);
    comp_section = new CSection(25);
    cout << "building my parking lot!!!\n";
    }

LotSection *ParkingLot::lookup_section(int typ) {
    LotSection *sec = NULL;
    switch(typ) {
        case T_REGULAR:
            sec = reg_section;
            break;
        case T_HANDICAPPED:
            sec = hc_section;
            break;
        case T_ELECTRIC:
            sec = el_section;
            break;
        case T_COMPACT:
            sec = comp_section;
            break;
        default:
            break;
    }
    return sec;
}

void ParkingLot::report_lot_state() {
    hc_section->report_state();
    reg_section->report_state();
    el_section->report_state();
    comp_section->report_state();
}

Car * ParkingLot::unpark(int id){
    Car *car_p;
    if ((car_p = hc_section->unpark_car(id)) != NULL)
        return car_p;
    if ((car_p = reg_section->unpark_car(id)) != NULL)
        return car_p;
    if ((car_p = el_section->unpark_car(id)) != NULL)
        return car_p;
    if ((car_p = comp_section->unpark_car(id)) != NULL)
        return car_p;
    return NULL;
}

Car * ParkingLot::unpark(int section_num, int id){
    LotSection *sec = lookup_section(id);
    if (sec != NULL) {
        return  sec->unpark_car(id);
    }
    return NULL;
}

void ParkingLot::park(Car *car){    
    cout << car->getCarName() << endl;
    LotSection *sec = lookup_section(car->car_type);
    
    if (sec != NULL) {
        sec->park_car(car);
    }
    else {
        cout << "Car doesn't run, you can't park here" << car->getCarName() << endl;
    }
}

void park_some_cars(ParkingLot &pl)
{
    int car_type;
    int vin;
    char operation;
    string line;
    Car *car_p;
    
    std::ifstream infile("/tmp/park_it.txt");    
 
    cout << "no problem!\n";
            
    while(std::getline(infile, line)) {
        //cout << endl << line << endl;
        std::istringstream iss(line);
        iss >> operation >> car_type >> vin;
       //cout << operation << car_p->car_type << endl;
        if (operation == 'u')
            pl.unpark(vin);
        else if (operation == 'p') {
            car_p = new Car(car_type, vin);
            pl.park(car_p);
        }
        else
            cout << "unsupported operation: " << operation << endl;
    }
    infile.close();

}



/*
 * 
 */
int main(int argc, char** argv) {
    
    cout << "hello park world\n";
    
    ParkingLot park_o_lot;
    
    park_some_cars(park_o_lot);
    
    park_o_lot.report_lot_state();

    return 0;
}

