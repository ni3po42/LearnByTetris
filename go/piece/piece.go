package piece

import (
	"math/rand"

	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/common"
)

type PieceId byte

type pieceData struct {
	field    [16]byte
	width    int
	height   int
	ordinals byte
	color    common.CellData
}

var rawPieces = map[PieceId]pieceData{
	'T': {
		field: [16]byte{
			0, 1, 0,
			1, 1, 1,
			0, 0, 0,
		},
		width:    3,
		height:   3,
		ordinals: 4,
		color:    1,
	},
	'O': {
		field: [16]byte{
			1, 1,
			1, 1,
		},
		width:    2,
		height:   2,
		ordinals: 1,
		color:    2,
	},
	'S': {
		field: [16]byte{
			0, 1, 1,
			1, 1, 0,
			0, 0, 0,
		},
		width:    3,
		height:   3,
		ordinals: 2,
		color:    3,
	},
	'Z': {
		field: [16]byte{
			1, 1, 0,
			0, 1, 1,
			0, 0, 0,
		},
		width:    3,
		height:   3,
		ordinals: 2,
		color:    4,
	},
	'J': {
		field: [16]byte{
			0, 0, 0,
			1, 1, 1,
			0, 0, 1,
		},
		width:    3,
		height:   3,
		ordinals: 4,
		color:    5,
	},
	'L': {
		field: [16]byte{
			0, 0, 0,
			1, 1, 1,
			1, 0, 0,
		},
		width:    3,
		height:   3,
		ordinals: 4,
		color:    6,
	},
	'I': {
		field: [16]byte{
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 0, 0, 0,
			0, 0, 0, 0,
		},
		width:    4,
		height:   4,
		ordinals: 2,
		color:    7,
	},
}

const idLookup = "TOSZJLI"

type Piece struct {
	row, col         int
	prevRow, prevCol int
	turns            byte
	prevTurns        byte
	active           bool

	height, width int
	id            PieceId
	ordinals      byte
	color         common.CellData
}

func NewPiece(args ...PieceId) *Piece {
	piece := Piece{
		id:        ' ',
		turns:     0,
		active:    false,
		row:       0,
		col:       0,
		prevCol:   0,
		prevRow:   0,
		prevTurns: 0,
		width:     0,
		height:    0,
		ordinals:  1,
		color:     0,
	}

	if len(args) > 0 {
		piece.id = args[0]
		piece.width = rawPieces[piece.id].width
		piece.height = rawPieces[piece.id].height
		piece.ordinals = rawPieces[piece.id].ordinals
		piece.color = rawPieces[piece.id].color
		piece.active = true
	}

	return &piece
}

func GetRandomPieceId() PieceId {
	index := rand.Intn(7)
	return PieceId(idLookup[index])
}

func Pieces() func() (*Piece, PieceId) {

	currentId := GetRandomPieceId()

	return func() (*Piece, PieceId) {
		nextId := GetRandomPieceId()
		currentPiece := NewPiece(currentId)
		currentId = nextId
		return currentPiece.Play(), nextId
	}
}

func (p *Piece) save() {
	p.prevRow = p.row
	p.prevCol = p.col
	p.prevTurns = p.turns
}

func (p Piece) Position() (int, int) {
	return p.row, p.col
}

func (p Piece) Dimensions() (int, int) {
	if p.turns%2 == 0 {
		return p.height, p.width
	} else {
		return p.width, p.height
	}
}

func (p Piece) GetID() PieceId {
	return p.id
}

func (p *Piece) Revert() {
	p.row = p.prevRow
	p.col = p.prevCol
	p.turns = p.prevTurns
}

func (p *Piece) Play() *Piece {
	p.row = 0
	p.col = (board.COLS / 2) - 1
	p.prevRow = 0
	p.prevCol = p.col
	p.turns = 0
	return p
}

func (p *Piece) Rotate(direction common.Direction) {
	p.save()

	switch direction {
	case common.LEFT:
		p.turns--
		p.turns = (p.turns + p.ordinals) % p.ordinals
	case common.RIGHT:
		p.turns++
		p.turns = p.turns % p.ordinals
	}
}

func (p *Piece) Drop() {
	p.save()
	p.row++
}

func (p *Piece) Move(direction common.Direction) {
	p.save()

	switch direction {
	case common.LEFT:
		p.col--
	case common.RIGHT:
		p.col++
	}
}

func (p *Piece) Commit() {
	p.active = false
}

func (p Piece) Scan(useNeturalPosition bool) func() (bool, common.ICellInfo) {

	row, col := p.row, p.col
	height, width := p.height, p.width
	turns := p.turns
	bound := p.width * p.height
	rawPieceData := rawPieces[p.id]
	active := p.active
	color := p.color

	var index int = 0

	return func() (bool, common.ICellInfo) {
		var r, c int
		var cellData common.CellData

		for {

			if index == bound {
				return false, nil
			}

			if rawPieceData.field[index] == 0 {
				index++
			} else {
				break
			}
		}

		if active {
			cellData = color | board.ACTIVE_MASK
		} else {
			cellData = color | board.STATIC_MASK
		}

		switch turns {
		case 0:
			r, c = index/width, index%width
		case 1:
			r, c = index%width, height-(index/width)-1
		case 2:
			r, c = (bound-index-1)/width, (bound-index-1)%width
		case 3:
			r, c = width-(index%width)-1, index/width
		}

		index++
		if useNeturalPosition {
			return true, board.NewScanData(r, c, cellData)
		} else {
			return true, board.NewScanData(r+row, c+col, cellData)
		}
	}
}
