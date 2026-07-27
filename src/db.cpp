#include "functions.h"

int open_db_connection() {
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(DB_HOST, DB_USER, DB_PASSWORD);

        con->setSchema(DB_SCHEMA);

        sql::Statement* stmt;
        stmt = con->createStatement();

        
        std::string selectDataSQL = "SELECT * FROM words";

        sql::ResultSet* res = stmt->executeQuery(selectDataSQL);

        // Loop through the result set and display data
        int count = 0;
        while (res->next()) {
            std::cout << " Definition " << ++count << ": " << res->getString("definition") << std::endl;
        }

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }

    return 0; 
}


