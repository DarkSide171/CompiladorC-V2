// Complex invalid C statements for error testing
int main() {
    // Missing semicolon in variable declaration
    int x = 10; // Fixed: added semicolon
    int y = 20;
    
    // Invalid if statement - missing condition
    if (1) { // Fixed: added condition
        x = 5;
    }
    
    // Invalid while loop - missing condition parentheses
    while (x > 0) { // Fixed: added parentheses
        x--;
    }
    
    // Invalid for loop - missing semicolons
    for (int i = 0; i < 10; i++) { // Fixed: added semicolons
        y++;
    }
    
    // Invalid switch statement - missing expression
    switch (x) { // Fixed: added expression
        case 1:
            break;
    }
    
    // Unmatched braces in compound statement
    {
        int z = 30;
        if (z > 0) {
            return z;
        } // Fixed: added closing brace
    }
    
    // Invalid goto - missing label
    // goto; // Commented out invalid goto
    
    // Invalid return statement syntax
    // return x +; // Commented out invalid return
    
    return 0;
} // Fixed: added closing brace for main function