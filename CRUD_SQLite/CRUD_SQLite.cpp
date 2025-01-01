#include <iostream>
#include <sqlite3.h>
#include <sstream>

using namespace std;

sqlite3* DB;

// Función para manejar errores
void checkError(int result, const string& errorMessage) {
    if (result != SQLITE_OK) {
        cout << errorMessage << endl;
        sqlite3_close(DB);
        exit(1);
    }
}

// Función para mostrar un mensaje al finalizar correctamente
void showSuccessMessage(const string& message) {
    cout << message << endl;
}

// Función para crear la tabla automáticamente
void createTable() {
    string sql = R"(CREATE TABLE IF NOT EXISTS USER (
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        NAME TEXT NOT NULL,
        LASTNAME TEXT NOT NULL,
        PHONE INTEGER NOT NULL,
        MAIL TEXT NOT NULL
    ))";
    int result = sqlite3_exec(DB, sql.c_str(), NULL, 0, 0);
    checkError(result, "Error al crear la tabla");
    showSuccessMessage("Tabla verificada/creada automáticamente");
}

// Función para insertar datos
void insertData() {
    string name, lastname, mail;
    int phone;

    cout << "Ingrese el nombre: ";
    cin >> name;
    cout << "Ingrese el apellido: ";
    cin >> lastname;
    cout << "Ingrese el teléfono: ";
    cin >> phone;
    cout << "Ingrese el correo: ";
    cin >> mail;

    string sql = "INSERT INTO USER (NAME, LASTNAME, PHONE, MAIL) VALUES ('" + name + "', '" + lastname + "', " + to_string(phone) + ", '" + mail + "')";
    int result = sqlite3_exec(DB, sql.c_str(), NULL, 0, 0);
    checkError(result, "Error al insertar los datos");
    showSuccessMessage("Datos insertados correctamente");
}

// Función para leer datos
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << "---------------------------------" << endl;
    return 0;
}

void readData() {
    string sql = "SELECT * FROM USER";
    int result = sqlite3_exec(DB, sql.c_str(), callback, 0, 0);
    checkError(result, "Error al leer los datos");
    showSuccessMessage("Consulta realizada correctamente");
}

// Función para actualizar datos
void updateData() {
    int id, phone;
    string name, lastname, mail;
    string sql = "UPDATE USER SET ";

    cout << "Ingrese el ID del usuario a actualizar: ";
    cin >> id;

    bool isFirstField = true;

    cout << "¿Desea actualizar el nombre? (s/n): ";
    char option;
    cin >> option;
    if (option == 's' || option == 'S') {
        cout << "Ingrese el nuevo nombre: ";
        cin >> name;
        if (!isFirstField) sql += ", ";
        sql += "NAME = '" + name + "'";
        isFirstField = false;
    }

    cout << "¿Desea actualizar el apellido? (s/n): ";
    cin >> option;
    if (option == 's' || option == 'S') {
        cout << "Ingrese el nuevo apellido: ";
        cin >> lastname;
        if (!isFirstField) sql += ", ";
        sql += "LASTNAME = '" + lastname + "'";
        isFirstField = false;
    }

    cout << "¿Desea actualizar el teléfono? (s/n): ";
    cin >> option;
    if (option == 's' || option == 'S') {
        cout << "Ingrese el nuevo teléfono: ";
        cin >> phone;
        if (!isFirstField) sql += ", ";
        sql += "PHONE = " + to_string(phone);
        isFirstField = false;
    }

    cout << "¿Desea actualizar el correo electrónico? (s/n): ";
    cin >> option;
    if (option == 's' || option == 'S') {
        cout << "Ingrese el nuevo correo electrónico: ";
        cin >> mail;
        if (!isFirstField) sql += ", ";
        sql += "MAIL = '" + mail + "'";
        isFirstField = false;
    }

    // Finalizar la consulta SQL con la condición de ID
    sql += " WHERE ID = " + to_string(id);

    // Mostrar la consulta SQL para depuración
    cout << "Consulta SQL: " << sql << endl;

    // Ejecutar la consulta
    int result = sqlite3_exec(DB, sql.c_str(), NULL, 0, 0);

    // Comprobar si hubo algún error en la ejecución de la consulta
    if (result != SQLITE_OK) {
        cerr << "Error al ejecutar la consulta: " << sqlite3_errmsg(DB) << endl;
    }
    else {
        showSuccessMessage("Datos actualizados correctamente");
    }
}


// Función para eliminar datos
void deleteData() {
    int id;
    cout << "Ingrese el ID del usuario a eliminar: ";
    cin >> id;

    string sql = "DELETE FROM USER WHERE ID = " + to_string(id);
    int result = sqlite3_exec(DB, sql.c_str(), NULL, 0, 0);
    checkError(result, "Error al eliminar los datos");
    showSuccessMessage("Datos eliminados correctamente");
}

// Función para mostrar el menú
void showMenu() {
    cout << "\n--- Menú CRUD SQLite ---" << endl;
    cout << "1. Insertar datos" << endl;
    cout << "2. Leer datos" << endl;
    cout << "3. Actualizar datos" << endl;
    cout << "4. Eliminar datos" << endl;
    cout << "5. Salir" << endl;
    cout << "Seleccione una opción: ";
}

int main() {
    // Abrir la base de datos
    string file = "./database.db";
    int result = sqlite3_open(file.c_str(), &DB);
    checkError(result, "Error al abrir la base de datos");

    // Crear la tabla automáticamente
    createTable();

    int option;
    do {
        showMenu();
        cin >> option;

        switch (option) {
        case 1:
            insertData();
            break;
        case 2:
            readData();
            break;
        case 3:
            updateData();
            break;
        case 4:
            deleteData();
            break;
        case 5:
            cout << "Saliendo del programa..." << endl;
            break;
        default:
            cout << "Opción no válida. Inténtelo de nuevo." << endl;
        }
    } while (option != 5);

    // Cerrar la base de datos
    sqlite3_close(DB);

    return 0;
}

