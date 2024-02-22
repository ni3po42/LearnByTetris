#include "piece.hpp"
#include "../test/utils.hpp"
#include <vector>

using tetris::piece::Piece;
using tetris::piece::ScanData;

void TEST_can_create_piece() {

    Piece p('T');

}

void TEST_can_scan_piece() {

    Piece p('T');

//010
//111

    std::vector<ScanData> expect {
        { 0, 1, 1 },
        { 1, 0, 1 },
        { 1, 1, 1 },
        { 1, 2, 1 }
    };

    auto gen = p.scan(true);
    int index = 0;
    while(auto c = gen()) {
        ScanData sc = expect.at(index);
        Expect(sc.col, c.value().col, "expected col: {}, got {}");
        Expect(sc.row, c.value().row, "expected row: {}, got {}");
        Expect(sc.data, c.value().data > 0 ? 1 : 0, "expected data: {}, got {}");
        index++;
    }

    Expect(4, index, "expected cell count: {}, but {}");
}

int main() {

    run(TEST_can_create_piece);
    run(TEST_can_scan_piece);
    return 0;
}