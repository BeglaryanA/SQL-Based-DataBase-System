#include "../header/dataSave.h"

void DataSave::save(Connect* connect_ptr) {
    connect_ptr->to_file();
}