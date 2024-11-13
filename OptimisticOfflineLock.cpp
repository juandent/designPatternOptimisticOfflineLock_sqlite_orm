#include <iostream>
#include "VersionedModel.h"


int main() {
    auto storage = initStorage("products.db");

    // Sync schema (create tables if they don’t exist)
    storage.sync_schema();

    // Insert a sample product (if it doesn’t exist)
    Product initialProduct{ -1, "Widget", 19.99, 1 };
    int productId = storage.insert(initialProduct);

    // Simulate fetching the product
    auto product = getProduct(storage, productId);
    std::cout << "Fetched product: " << product.name << " at version " << product.version << "\n";

    // Try to update with the correct version
    bool success = updateProduct(storage, product.id, "Updated Widget", 25.99, product.version);
    if (success) {
        std::cout << "Update successful.\n";
    }
    else {
        std::cout << "Update failed due to version conflict.\n";
    }

    // Simulate a conflicting update by another process
    Product conflictingUpdate = getProduct(storage, productId);
    conflictingUpdate.name = "Conflicting Widget";
    conflictingUpdate.version += 1;  // Increment version to simulate an external change
    storage.update(conflictingUpdate);  // Direct update without version check

    // Try to update again with the original version
    success = updateProduct(storage, product.id, "Another Update", 29.99, product.version);
    if (!success) {
        std::cout << "Conflict detected, update not applied.\n";
    }

    return 0;
}
