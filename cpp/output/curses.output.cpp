#include "output.hpp"
#include <curses.h>
#include <string>
#include <format>

#include "../board/board.hpp"
#include "../piece/piece.hpp"

namespace tetris::io::curses {

    using tetris::gameboard::Board;
    using tetris::gameboard::CellData;
    using tetris::gameboard::COLOR_MASK;
    using tetris::gameboard::EMPTY_MASK;
    using tetris::gameboard::COLS;
    using tetris::gameboard::ROWS;
    using tetris::piece::Piece;
    using PieceScanData = tetris::piece::ScanData;
    using BoardScanData = tetris::gameboard::ScanData;

    constexpr unsigned int SCORE_OFFSET_ROW = 7;
    constexpr unsigned int SCORE_OFFSET_COL = 4;
    constexpr unsigned int LEVEL_OFFSET_ROW = 10;
    constexpr unsigned int LEVEL_OFFSET_COL = 4;
    constexpr unsigned int GAMEOVER_OFFSET_ROW = 10;
    constexpr unsigned int GAMEOVER_OFFSET_COL = 22;
    constexpr unsigned int BOARD_OFFSET_ROW = 1;
    constexpr unsigned int BOARD_OFFSET_COL = 7;
    constexpr unsigned int NEXTPIECE_OFFSET_ROW = 3;
    constexpr unsigned int NEXTPIECE_OFFSET_COL = 2;
    constexpr unsigned int DEBUG_OFFSET_ROW = 13;
    constexpr unsigned int DEBUG_OFFSET_COL = 4;

    class CursesOutputService : public IOutputService {

        private:            
            std::optional<Piece> nextPieceBuffer;

            static void drawText(const std::string& text, int row, int col) {                
                mvprintw(row, col, "%s", text.c_str());
            }

            static void drawCell(int row, int col, CellData data, int rowOffset, int colOffset) {
                
                int color = (data & COLOR_MASK);
                
                if (color != 0) {
                    attron(COLOR_PAIR(color));
                    attron(A_REVERSE);
                } else {
                    attron(COLOR_PAIR(8));
                }
                
                mvprintw(row + rowOffset, (col + colOffset) * 2, "  ");
                
                if (color != 0) {
                    attroff(A_REVERSE);
                    attroff(COLOR_PAIR(color));
                } else {
                    attroff(COLOR_PAIR(8));
                }
                
            }

            static void drawInteger(int value, int row, int col) {               
                drawText(std::format("{}", value), row, col);
            }

            static void renderNextPiece_erase(PieceScanData scanData) {
                auto [row, col, _] = scanData;
                drawCell(row, col, EMPTY_MASK, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
            }

            static void renderNextPiece_draw(PieceScanData scanData) {
                auto [row, col, data] = scanData;
                drawCell(row, col, data, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
            }

            void renderNextPiece(char nextPieceId, const Piece& currentPiece) {
                
                if (!nextPieceBuffer.has_value() || nextPieceBuffer->getId() != nextPieceId) {
                    nextPieceBuffer = Piece(nextPieceId);
                    
                    currentPiece
                        .scan(true)
                        .loop(renderNextPiece_erase);

                    nextPieceBuffer->scan(true)
                        .loop(renderNextPiece_draw);
                }
            }

            void gameBoard(const Board& board) {
            
                auto boardGen = board.scan(0,0,ROWS, COLS);

                while(auto cell = boardGen()) {                    
                    drawCell(cell->row, cell->col, cell->data, BOARD_OFFSET_ROW, BOARD_OFFSET_COL);
                }
                
                move(0, 27);
            }


        public:

            CursesOutputService() {

                initscr();			/* Start curses mode 		*/
                noecho();			/* Don't echo() while we do getch */

                start_color();

                init_pair(1, COLOR_RED, COLOR_BLACK);
                init_pair(2, COLOR_GREEN, COLOR_BLACK);
                init_pair(3, COLOR_YELLOW, COLOR_BLACK);
                init_pair(4, COLOR_BLUE, COLOR_BLACK);
                init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
                init_pair(6, COLOR_CYAN, COLOR_BLACK);
                init_pair(7, COLOR_WHITE, COLOR_BLACK);
                init_pair(8, COLOR_BLACK, COLOR_BLACK);

                clear();
                drawText("SCORE", SCORE_OFFSET_ROW, SCORE_OFFSET_COL);
                drawText("LEVEL", LEVEL_OFFSET_ROW, LEVEL_OFFSET_COL);
                refresh();
            }

            virtual ~CursesOutputService() {
                fprintf(stderr, "~out\n");
                endwin();
                fprintf(stderr, "~out2\n");
            }

            virtual void clear() override {
                move(10, 20);
                ::clear();
                refresh();
            };

            virtual void render(Status status, const Board& board) override {
                auto [currentPiece, nextPieceId] = status.pieceInfo;
                renderNextPiece(nextPieceId, currentPiece);
                gameBoard(board);
                drawInteger(status.score, SCORE_OFFSET_ROW + 1, SCORE_OFFSET_COL);
                drawInteger(status.level, LEVEL_OFFSET_ROW + 1, LEVEL_OFFSET_COL);

                if (status.isGameOver)
                {
                    drawText("GAME OVER", GAMEOVER_OFFSET_ROW, GAMEOVER_OFFSET_COL);   
                }
                
                refresh();
            };

            virtual void debug(const std::string& message) override {
                drawText(message, DEBUG_OFFSET_ROW, DEBUG_OFFSET_COL);
                refresh();
            };
            
    };
}