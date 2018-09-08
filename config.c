/* config.c */

/* Geräte Namen (IDs) */
#define ID0 "\"TIP-00078107-03-02\""
#define ID1 "\"TIP-00078155-03-02\""
#define ID2 "\"TIP-02078095-03-07\""
#define ID3 "\"TIP-00078327-05-01\""
#define ID4 "\"PLATZHALTER\""
#define ID5 "\"PLATZHALTER\""
#define ID6 "\"PLATZHALTER\""
#define ID7 "\"PLATZHALTER\""

/* Anzahl der aktiven IDs (für reduce2importantdata) wichtig */
#define INSTALLED_IDS 2
/* Maximale Anzahl der Zeilen, die die Eingabedatei hat (kommt auf das
 * Intervall, das man ausgewählt hat an). Bei einem Intervall von
 * 5 Minuten wären es bei 2 Geräten circa 1700 Zeilen pro Datei(Tag)
 *
 * Diese Zahl wird anschließende mal der INSTALLED_IDS genommen im
 * Programm (INSTALLED_IDS*MAX_LINE_NUMBERS_PER_DEVICE)
 */
#define MAX_LINE_NUMBERS_PER_DEVICE 1024

/* Benutzerdefinierte IDs(Costum IDs) */
/* Beispiel: CID0 "\"Verpackungsmaschine\"" */
#define CID0 "\"ID A\""
#define CID1 "\"ID B\""
#define CID2 "\"ID C\""
#define CID3 "\"ID D\""
#define CID4 "\"PLATZHALTER\""
#define CID5 "\"PLATZHALTER\""
#define CID6 "\"PLATZHALTER\""
#define CID7 "\"PLATZHALTER\""






/* End of Config Don't change anything here */
/* Removing the magic numbers */
#define TIP_INPUT_LENGTH 30			//max length of the standard device name
#define CUSTOMID_LENGTH 30			//max length of a custom name for a device
#define CONVERTED_TIME_LENGTH 30	//max length of the converted time string
#define EPOCH_TIME_LENGTH 14		//The stringlength of the Epoch-input-time usually 14

/* Colors yay thanks to stackoverflow.com */
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"
