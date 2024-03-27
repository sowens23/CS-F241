    String s = extractArg(2,cmdline);
    for (size_t i = 0; i<s.length(); i++) {
        char c = s[i];
        if ((c >> 7) == 0 { // ascii
        Serial.print(c);
    }
    else { // High unicode: loop over bytes of codepoint
        unsigned char u = c;
        do {
            Serial.print((char)u); // Print leading byte
            i++; // move down string
            u = s[i];
        } 
