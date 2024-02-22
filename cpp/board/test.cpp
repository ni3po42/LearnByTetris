#include "board.hpp"
#include "../test/utils.hpp"

using tetris::gameboard::Board;

void TEST_can_create_board() {

    Board board;

}

void TEST_can_scan_board() {
    
    using tetris::gameboard::COLS;
    using tetris::gameboard::ROWS;
    using tetris::gameboard::SIZE;
    using tetris::gameboard::STATIC_MASK;
    using tetris::gameboard::WALL_MASK;
    using tetris::gameboard::WALL_COLOR;
    using tetris::gameboard::EMPTY_MASK;

    using tetris::gameboard::ScanData;

    using tetris::generator::Generator;

    Board board;
    board.clear();

    auto gen = board.scan(0, 0, ROWS, COLS);
    int index = 0;
    

    for(auto next = gen(); next; index++, next = gen()) {
        
        auto actual = next
                        .value_or(ScanData {-1, -1, -1});

        if (actual.col == 0 || actual.col + 1 == COLS){
            Expect((STATIC_MASK | WALL_MASK | WALL_COLOR), actual.data, "data was not expected");
        } else if (actual.row + 1 == ROWS) {
            Expect((STATIC_MASK | WALL_MASK | WALL_COLOR), actual.data, "data was not expected");
        } else {
            Expect(index % COLS, actual.col, "expected {} for col, got {}");
            Expect(index / COLS, actual.row, "expedted {} for row, got {}");
            Expect(EMPTY_MASK, actual.data, "expedted {} for data, got {}");
        }       
    }
    Expect(SIZE, index, "scan returned more cells than in board");
}

int main() {

    run(TEST_can_create_board);
    run(TEST_can_scan_board);
    return 0;
}