// Edge cases for statement parsing
int main() {
    // Empty compound statement
    {
    }
    
    // Nested empty statements
    if (1) {
        ;
    }
    
    // Multiple semicolons (empty statements)
    ;;;
    
    // Single statement without braces
    if (1)
        return 1;
    
    // Deeply nested statements
    if (1) {
        if (2) {
            if (3) {
                if (4) {
                    return 4;
                }
            }
        }
    }
    
    // Switch with no cases
    switch (1) {
    }
    
    // For loop with empty parts
    for (;;) {
        break;
    }
    
    // While with constant condition
    while (0) {
        // This should never execute
    }
    
    // Do-while with false condition
    do {
        // This executes once
    } while (0);
    
    // Return without expression in void context would be here
    // but main must return int
    return 0;
}

// Function with only return
void empty_function() {
    return;
}