// Complex valid C statements for comprehensive testing
int main() {
    // Variable declarations and assignments
    int x = 10, y = 20, z;
    
    // If-else statements
    if (x > y) {
        z = x;
    } else if (x < y) {
        z = y;
    } else {
        z = 0;
    }
    
    // While loop
    while (x > 0) {
        x--;
        if (x == 5) {
            break;
        }
        if (x % 2 == 0) {
            continue;
        }
    }
    
    // For loop
    for (int i = 0; i < 10; i++) {
        if (i == 3) {
            goto end_loop;
        }
        y += i;
    }
    
    end_loop:
    
    // Do-while loop
    do {
        z++;
    } while (z < 5);
    
    // Switch statement
    switch (z) {
        case 1:
            return 1;
        case 2:
        case 3:
            return z;
        default:
            break;
    }
    
    // Nested compound statements
    {
        int local_var = 100;
        {
            int nested_var = local_var + z;
            if (nested_var > 50) {
                return nested_var;
            }
        }
    }
    
    return 0;
}