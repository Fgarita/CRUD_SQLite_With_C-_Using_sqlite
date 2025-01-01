#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

sqlite3* DB;

// Función para manejar errores
void checkError(int result, const string& errorMessage) {
    if (result != SQLITE_OK) {
        cerr << "\n[ERROR] " << errorMessage << "\nDetalles: " << sqlite3_errmsg(DB) << "\n" << endl;
        sqlite3_close(DB);
        exit(1);
    }
}

// Función para mostrar un mensaje al finalizar correctamente
void showSuccessMessage(const string& message) {
    cout << "\n[✔] " << message << "\n" << endl;
}

// Función para crear la tabla automáticamente
void createTable() {
    string sql = R"(CREATE TABLE IF NOT EXISTS USER (
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        NAME TEXT NOT NULL,
        LASTNAME TEXT NOT NULL,
        PHONE TEXT NOT NULL,
        MAIL TEXT NOT NULL
    ))";

    int result = sqlite3_exec(DB, sql.c_str(), NULL, 0, 0);
    cout << "=======================================" << endl;
    checkError(result, "Error al crear la tabla");
    showSuccessMessage("Tabla verificada/creada automáticamente");
    cout << "=======================================" << endl;
}

// Función para verificar si un ID existe en la base de datos
bool doesIDExist(int id) {
    string sql = "SELECT COUNT(*) FROM USER WHERE ID = ?";
    sqlite3_stmt* stmt;

    int result = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    cout << "=======================================" << endl;
    checkError(result, "Error al preparar la consulta para verificar el ID");
    cout << "=======================================" << endl;

    sqlite3_bind_int(stmt, 1, id);

    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        cout << "=======================================" << endl;
        cerr << "\n[ERROR] Error al verificar el ID: " << sqlite3_errmsg(DB) << "\n" << endl;
        cout << "=======================================" << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    return count > 0;
}

// Función para insertar datos
void insertData() {
    string name, lastname, mail, phone;
    cout << "=======================================" << endl;

    cout << "\n[INFO] Ingrese los datos del usuario:\n";
    cout << "=======================================" << endl;

    cout << "Nombre: ";
    cin >> ws;
    getline(cin, name);
    cout << "=======================================" << endl;

    cout << "Apellido: ";
    getline(cin, lastname);
    cout << "=======================================" << endl;

    cout << "Teléfono: ";
    getline(cin, phone);
    cout << "=======================================" << endl;

    cout << "Correo: ";
    getline(cin, mail);
    cout << "=======================================" << endl;

    string sql = "INSERT INTO USER (NAME, LASTNAME, PHONE, MAIL) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    int result = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    checkError(result, "Error al preparar la consulta");

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, lastname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, phone.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, mail.c_str(), -1, SQLITE_STATIC);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        cerr << "\n[ERROR] Error al insertar los datos: " << sqlite3_errmsg(DB) << "\n" << endl;
    }
    else {
        showSuccessMessage("Datos insertados correctamente");
    }
    sqlite3_finalize(stmt);
}

// Función para leer datos
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << left << setw(15) << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << "---------------------------------" << endl;
    return 0;
}

void readData() {
    cout << "=======================================" << endl;
    cout << "\n[INFO] Leyendo todos los datos:\n";
    cout << "=======================================" << endl;
    string sql = "SELECT * FROM USER";
    int result = sqlite3_exec(DB, sql.c_str(), callback, 0, 0);
    cout << "=======================================" << endl;
    checkError(result, "Error al leer los datos");
    showSuccessMessage("Consulta realizada correctamente");
    cout << "=======================================" << endl;
}

// Función para actualizar datos
void updateData() {
    int id;
    cout << "=======================================" << endl;
    cout << "\n[INFO] Ingrese el ID del usuario a actualizar: ";
    cin >> id;

    if (!doesIDExist(id)) {
        cout << "=======================================" << endl;
        cerr << "\n[ERROR] El ID ingresado no existe en la base de datos.\n" << endl;
        cout << "=======================================" << endl;
        return;
    }

    string sql = "UPDATE USER SET ";
    sqlite3_stmt* stmt;

    string fields[4] = { "NAME", "LASTNAME", "PHONE", "MAIL" };
    string newValues[4];
    bool isFirstField = true;

    for (int i = 0; i < 4; i++) {
        cout << "=======================================" << endl;
        cout << "¿Desea actualizar " << fields[i] << "? (s/n): ";
        cout << "=======================================" << endl;
        char option;
        cin >> option;
        if (option == 's' || option == 'S') {
            cout << "=======================================" << endl;
            cout << "Ingrese el nuevo valor para " << fields[i] << ": ";
            cout << "=======================================" << endl;
            cin >> ws;
            getline(cin, newValues[i]);
            if (!isFirstField) sql += ", ";
            sql += fields[i] + " = ?";
            isFirstField = false;
        }
    }

    if (isFirstField) {
        cout << "=======================================" << endl;
        cout << "\n[INFO] No se actualizó ningún campo.\n" << endl;
        cout << "=======================================" << endl;
        return;
    }

    sql += " WHERE ID = ?";
    int result = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    cout << "=======================================" << endl;
    checkError(result, "Error al preparar la consulta de actualización");
    cout << "=======================================" << endl;

    int paramIndex = 1;
    for (const auto& value : newValues) {
        if (!value.empty()) {
            sqlite3_bind_text(stmt, paramIndex++, value.c_str(), -1, SQLITE_STATIC);
        }
    }
    sqlite3_bind_int(stmt, paramIndex, id);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        cout << "=======================================" << endl;
        cerr << "\n[ERROR] Error al actualizar los datos: " << sqlite3_errmsg(DB) << "\n" << endl;
        cout << "=======================================" << endl;
    }
    else {
        cout << "=======================================" << endl;
        showSuccessMessage("Datos actualizados correctamente");
        cout << "=======================================" << endl;
    }
    sqlite3_finalize(stmt);
}

// Función para eliminar datos
void deleteData() {
    int id;
    cout << "=======================================" << endl;
    cout << "\n[INFO] Ingrese el ID del usuario a eliminar: ";
    cin >> id;

    if (!doesIDExist(id)) {
        cout << "=======================================" << endl;
        cerr << "\n[ERROR] El ID ingresado no existe en la base de datos.\n" << endl;
        cout << "=======================================" << endl;
        return;
    }

    string sql = "DELETE FROM USER WHERE ID = ?";
    sqlite3_stmt* stmt;

    int result = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    cout << "=======================================" << endl;
    checkError(result, "Error al preparar la consulta de eliminación");
    cout << "=======================================" << endl;

    sqlite3_bind_int(stmt, 1, id);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        cout << "=======================================" << endl;
        cerr << "\n[ERROR] Error al eliminar los datos: " << sqlite3_errmsg(DB) << "\n" << endl;
        cout << "=======================================" << endl;
    }
    else {
        cout << "=======================================" << endl;
        showSuccessMessage("Datos eliminados correctamente");
        cout << "=======================================" << endl;
    }
    sqlite3_finalize(stmt);
}

// Función para mostrar el menú
void showMenu() {
    cout << "\n========== Menú CRUD SQLite ==========" << endl;
    cout << "1. Insertar datos" << endl;
    cout << "2. Leer datos" << endl;
    cout << "3. Actualizar datos" << endl;
    cout << "4. Eliminar datos" << endl;
    cout << "5. Salir" << endl;
    cout << "=======================================" << endl;
    cout << "Seleccione una opción: ";
}

int main() {
    string file = "./database.db";
    int result = sqlite3_open(file.c_str(), &DB);
    cout << "=======================================" << endl;
    checkError(result, "Error al abrir la base de datos");
    cout << "=======================================" << endl;

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
            cout << "=======================================" << endl;
            cout << "\n[INFO] Saliendo del programa...\n" << endl;
            cout << "=======================================" << endl;
            break;
        default:
            cout << "=======================================" << endl;
            cout << "\n[ERROR] Opción no válida. Inténtelo de nuevo.\n" << endl;
            cout << "=======================================" << endl;
        }
    } while (option != 5);

    sqlite3_close(DB);
    return 0;
}
