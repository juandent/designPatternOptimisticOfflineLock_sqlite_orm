#pragma once
#include <sqlite_orm/sqlite_orm.h>
#include <string>

struct Product {
    int id;
    std::string name;
    double price;
    int version;  // This column will track the version for optimistic locking
};

auto initStorage(const std::string& dbPath) {
    using namespace sqlite_orm;

    return make_storage(
        dbPath,

        make_table("Products",
            make_column("id", &Product::id, primary_key().autoincrement()),
            make_column("name", &Product::name),
            make_column("price", &Product::price),
            make_column("version", &Product::version)
        )
    );
}

Product getProduct(decltype(initStorage(""))& storage, int productId) {
    return storage.get<Product>(productId);
}


bool updateProduct(decltype(initStorage(""))& storage, int productId, const std::string& newName, double newPrice, int currentVersion) {
    using namespace sqlite_orm;

    // Check if the product's current version matches the provided version
    auto productInDb = storage.get_pointer<Product>(productId);
    if (!productInDb) {
        std::cerr << "Product not found.\n";
        return false;
    }

    // Check for version mismatch
    if (productInDb->version != currentVersion) {
        std::cerr << "Version conflict detected. Another user has modified this record.\n";
        return false;
    }

    // If versions match, update the record and increment the version
    productInDb->name = newName;
    productInDb->price = newPrice;
    productInDb->version += 1;  // Increment version for optimistic lock

    // Update the record in the database
    storage.update(*productInDb);

    std::cout << "Product updated successfully.\n";
    return true;
}

