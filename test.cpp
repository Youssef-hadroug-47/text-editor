// C++ program to illustrate
// mbrtowc() function
#include <bits/stdc++.h>
using namespace std;

// Function to convert multibyte
// sequence to wide character
void print_(const char* s)
{
    // initial state
    mbstate_t ps = mbstate_t();

    // length of the string
    int length = strlen(s);

    const char* n = s + length;
    int len;
    wchar_t pwc;

    // printing each bytes
    while ((len = mbrtowc(&pwc, s, n - s, &ps)) > 0) {
        wcout << "Next " << len << 
        " bytes are the character " << pwc << '\n';
        s += len;
    }
}

// Driver code
int main()
{
    setlocale(LC_ALL, "en_US.utf8");

    // UTF-8 narrow multibyte encoding
    const char* str = u8"z\u00df\u6c34\U0001d10b";

    print_(str);
}
