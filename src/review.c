#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#include <ncurses.h>

#include "main.h"
#include "card.h"
#include "review.h"

#define INFO_WIN_H	4
#define INFO_WIN_W	20
#define	CARD_WIN_H	4
#define	CARD_WIN_W	20

#define	draw_infowin()	mvwprintw(infowin, 0, 0, "card %d/%d\nright: %d\nwrong: %d", cardpos + 1, card_list_len, right_cards, wrong_cards)
#define draw_frontwin()	mvwaddstr(frontwin, 0, 0, fronttext)
#define	draw_backwin()	mvwaddstr(backwin, 0, 0, backtext)

static WINDOW *infowin, *frontwin, *backwin;
static char *fronttext, *backtext;

// Controls the visibility of the back of the card being viewed
static bool showback = false;

// No. of cards marked right or wrong
static int right_cards, wrong_cards;

// Index of current card being read
static int cardpos;

static int init_windows(void);
static void resize_windows(void);

void start_review_mode(void)
{
	if (init_windows() != 0)
		end_program(errno);

	// Keeps track of whether or not the user has marked all cards right
	bool all_cards_right = true;

	// Review loop
	for (;;)
	{
		all_cards_right = true;
		for (int i = 0; i < card_list_len; i++)
		{
			// Don't display cards that haven't been marked for review
			if (review_list[i] == false)
				continue;
	
			// Update global variables used for drawing
			cardpos = i;
			fronttext = card_list[i]->front;
			backtext = card_list[i]->back;

			// Hide the back of the card
			showback = false;
			wclear(backwin);
			wrefresh(backwin);

			// Display the front of the card
			wclear(frontwin);
			draw_frontwin();
			wrefresh(frontwin);

			// Display misc info
			wclear(infowin);
			draw_infowin();
			wrefresh(infowin);

			get_input:
			switch (tolower(wgetch(frontwin)))
			{
				case 'j':
				{
					// Toggle back of card visibility
					if ((showback = showback ? false : true) == true)
						draw_backwin();
					else
						wclear(backwin);
					wrefresh(backwin);
					goto get_input;
				}
				case 'k':
				{
					// Mark card as wrong
					review_list[i] = true;
					all_cards_right = false;
					wrong_cards++;
					break;
				}
				case 'l':
				{
					// Mark card as right
					review_list[i] = false;
					right_cards++;
					break;
				}
				case KEY_RESIZE:
				{
					resize_windows();
					goto get_input;
				}
				case 'q':
				{
					end_program(0);
				}
				default:
					goto get_input;
			}
		}

		// If the user marked all cards as right this review, review every card again
		if (all_cards_right)
			for (int i = 0; i < card_list_len; i++)
				review_list[i] = true;
	}
}

/*
 * creates review mode windows
 * 
 * returns errno on error
 */
static int init_windows(void)
{
	int mx, my;

	getmaxyx(stdscr, my, mx);
	infowin = newwin(INFO_WIN_H, INFO_WIN_W, 0, 0);
	frontwin = newwin(CARD_WIN_H, CARD_WIN_W, my / 2 - 1 - CARD_WIN_H, mx / 2 - CARD_WIN_W / 2);
	backwin = newwin(CARD_WIN_H, CARD_WIN_W, my / 2 + 1, mx / 2 - CARD_WIN_W / 2);

	if (infowin == NULL || frontwin == NULL || backwin == NULL)
	{
		perror("newwin");
		return errno;
	}

	// Enable special key detection for frontwin, the window used for input
	if ((keypad(frontwin, true)) == ERR)
	{
		perror("keypad");
		return errno;
	}

	return 0;
}

static void resize_windows(void)
{
	int mx, my;

	getmaxyx(stdscr, my, mx);

	wclear(infowin);
	wclear(frontwin);
	wclear(backwin);
	wrefresh(infowin);
	wrefresh(frontwin);
	wrefresh(backwin);

	mvwin(frontwin, my / 2 - 1 - CARD_WIN_H, mx / 2 - CARD_WIN_W / 2);
	mvwin(backwin, my / 2 + 1, mx / 2 - CARD_WIN_W / 2);

	draw_infowin();
	draw_frontwin();
	wrefresh(infowin);
	wrefresh(frontwin);

	if (showback)
	{
		draw_backwin();
		wrefresh(backwin);
	}
}