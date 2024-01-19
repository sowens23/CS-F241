# CS-241 Computer Hardware (Circuits)
## Class Notes and Homework Repository :dizzy: :earth_americas:
  ```
  #include <iostream>
  int main() {
    std::cout << "Hello traveller, here's a towel for protection. Take care of yourself out there." << std::endl;
    return 0;
  }
  ```
  ![Hitchhikers Guide to the Galaxy](https://github.com/sowens23/CS-F311/blob/main/inclasscoding/week1/tenor.gif)

### Class Repositories and References
  - [sowens23-GitHub](https://github.com/sowens23)
  - [GitHubPortal](https://github.com/sowens23/Newbie-Gains/blob/main/README.md)
  - [CS-F241 Class Homepage](https://docs.google.com/document/d/e/2PACX-1vR__87c237BHRn4d2LxpBF-sVQqBQhSAXdCylhO5xk2_HO4igC0espns_l858m7pfg20pr9nr-OG2_a/pub)

### Big Notes
  - 

# Class Notes and Assignments
  | Weekly Notes | Assignments | Other Notes |
  | --- | --- | --- |
  | [Week-1](#Week-1) | | |

# Week-1
[Top](#TOP)
## 2024-01-19
  - Parallel give you redundancy
  - Series gives you reliability
  - There must be a *difference* in voltage to flow. Meaning if both ends are 5V's, then there will be no flow, and nothing will turn on.

  ### Color Coding
  - AC
    - Black/Red/Brown is hot
    - Green is ground
    - Blue/White are neutral
  - DC
    - Black is Ground
    - Red is Hot (+V)

  ### Messing Around with Arduino
  ``` 
  // It's good practice to assign pins as const ints as such

  void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPIn, INPUT_PULLU);
  }

  void loop () {
    int newButton-digitalread(buttonPin);
    if (newButton==0 && oldButton!=0) {
      lightOn=!lightOn;
    }
    oldButton-newButton
    digitalWrite(ledPin, lightOn);
  }
  ```  