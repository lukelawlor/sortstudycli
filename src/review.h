#define	MAX_INFO_CARDS		999
#define	MAX_INFO_RIGHTWRONG	99999
extern int right_cards, wrong_cards, cardpos, numcards;
extern char lastaction[23];
extern bool is_full_review, review_finished;
void start_review_mode(bool, bool, bool);
void prevent_small_screen(int, int);
