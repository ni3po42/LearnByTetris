package common

type CellData int

type Direction byte

const (
	LEFT Direction = iota
	RIGHT
)

type ICellInfo interface {
	Position() (int, int) //row, col
	Data() CellData
	HasCollision() bool
}

type IRectangle interface {
	Position() (int, int)   //row, col
	Dimensions() (int, int) //height, width
}

type CellRange struct {
	Row    int
	Col    int
	Height int
	Width  int
}

func (c CellRange) Position() (int, int) {
	return c.Row, c.Col
}

func (c CellRange) Dimensions() (int, int) {
	return c.Height, c.Width
}
