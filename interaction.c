#include "board.h"
#include "logic.h"
#include <notcurses/nckeys.h>
#include <notcurses/notcurses.h>

void blank_state() {
  resetReach();
  resetKills();
}

/* May not work under different blitters (works for NCBLIT_PIXEL) */
bool handle_actions(ncinput *ni, V2 cell_size, V2 dims) {
  bool ui_changed = false;
  if (ni->id == NCKEY_BUTTON1) {
    ui x = ni->xpx + ni->x * cell_size.x;
    ui y = ni->ypx + ni->y * cell_size.y;
    if (x <= dims.x && y <= dims.y) {
      u8 col = (x / (dims.x / 10));
      u8 row = (y / (dims.y / 10));
      int pawn = col + row * 10;
      if (col < 10 && row < 10) {

        ui_changed = true;
        if (isPlayerPawn(col, row)) {
          setCurrPawn(pawn);
          int currPawnIsKing = isKingPawn(col, row);
          setKingPawn(currPawnIsKing);

          blank_state();
          setReach(pawn, true);

          handleMoves(col, row);
          handleKillingMoves(pawn, &(Move){_NONMOVE, 0, NULL});
          if (currPawnIsKing) {
            handleKingKillingMoves(pawn);
          } else {
            handlePromotion();
          }

        } else if (isReachable(col, row)) {

          if (isKillingMoveOption(col, row)) {
            Move *m = &getKillBuffer()[col + row * 10];
            do {
              setBoard(m->kill, 0);
            } while ((m = m->prev) != NULL);
          }
          movePawn(getCurrPawn(), col + row * 10);

          blank_state();
          nextPlayer();
        }
      }
    }
  }
  return ui_changed;
}
