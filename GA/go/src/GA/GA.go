package GA

import (
	//"fmt"
	"errors"
	"sort"
)

type Gene []interface{}

type GA struct {
	// parameters
	SurvivorsLimit   uint
	NumberOfChildren uint

	// user defined funcs
	CrossOverFunc   func(Gene, Gene) Gene
	MutationFunc    func(Gene) Gene
	CalcFitnessFunc func(Gene) float64
	ProduceGeneFunc func() Gene

	// internal status
	Offsprings []Gene
}

const defaultSurvivorsLimit = 10
const defaultNumberOfChildren = 4

func (g *GA) Less(i, j int) bool {
	if g.CalcFitnessFunc == nil {
		panic("what are you trying to do?")
	}
	v1 := g.CalcFitnessFunc(g.Offsprings[i])
	v2 := g.CalcFitnessFunc(g.Offsprings[j])
	return v1 < v2
}

func (g *GA) Swap(i, j int) {
	g.Offsprings[i], g.Offsprings[j] = g.Offsprings[j], g.Offsprings[i]
}

func (g *GA) Len() int {
	return len(g.Offsprings)
}

func (g *GA) Populate(popTop uint) error {
	if g.ProduceGeneFunc == nil {
		return errors.New("gene factory function is nil")
	}
	for i := uint(0); i < popTop; i++ {
		g.Offsprings = append(g.Offsprings, g.ProduceGeneFunc())
	}
	return nil
}

func (g *GA) Evolve(howManyTimes uint) error {
	if g.CrossOverFunc == nil ||
		g.MutationFunc == nil ||
		g.CalcFitnessFunc == nil {
		return errors.New("GA is not configured properly")
	}

	tmp := make([]Gene, 0)
	for n := uint(0); n < howManyTimes; n++ {
		// order by fitness and decimate
		// crossover + reproduction
		// crossover
		sort.Sort(g)
		for i := 0; i < len(g.Offsprings)-1; i++ {
			newg := g.CrossOverFunc(g.Offsprings[i], g.Offsprings[i+1])
			tmp = append(tmp, g.Offsprings[i])
			tmp = append(tmp, g.Offsprings[i+1])
			for j := uint(0); j < g.NumberOfChildren; j++ {
				tmp = append(tmp, g.MutationFunc(newg))
			}
		}
		copy(g.Offsprings, tmp)
		tmp = nil
	}
	if g.SurvivorsLimit < uint(len(g.Offsprings)) {
		sort.Sort(g)
		g.Offsprings = g.Offsprings[:g.SurvivorsLimit]
	}
	return nil
}

func (g *GA) Winner() Gene {
	sort.Sort(g)
	return g.Offsprings[0]
}

func New() *GA {
	return &GA{
		defaultSurvivorsLimit,
		defaultNumberOfChildren,
		nil, // crossover
		nil, // mutation
		nil, // fitness
		nil, // produce
		make([]Gene, 0),
	}
}
