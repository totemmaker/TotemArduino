/**
 * Displaying scrolling text on LabBoard display
 */
##include <TotemLabBoard.h>
TotemLabBoard LB;
const char text[] = "This is a long scrolling text trying to fit in a display. ";
size_t textStart = 0;

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Off all LED
  LB.led.off();
}

void loop() {
  // Prepare display buffer
  char display[10];
  // Fill display buffer with shifter text
  for (int i=0; i<9; i++) {
    // Get next letter to place in buffer
    size_t nextLetter = (textStart + i) % (sizeof(text)-1);
    // Put text charater to buffer
    display[i] = text[nextLetter];
  }
  // Increment text start
  textStart = (textStart + 1) % (sizeof(text)-1);
  // End string with null character.
  // Required for print function to detect length of text inside "display"
  display[9] = '\0';
  // Print display buffer to LabBoard
  LB.display.print(display);
  // Refresh each 0.4 second
  delay(300);
}
