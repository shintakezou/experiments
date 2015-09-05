package main

// set GOPATH properly or move GA.

import (
	"GA"

	"flag"
	"fmt"
	"math"
	"math/rand"
)

const mutationProb = 0.5
const initialPopulation = 50
const numOfIterations = 1000
const defaultMinVal = 0.0
const defaultMaxVal = 10.0
const defaultStep = 0.1

var initialPop = flag.Uint("pop", initialPopulation, "initial numbers of random individuals")
var iterations = flag.Uint("iter", numOfIterations, "number of iterations (generations)")
var printWinner = flag.Bool("printwinner", false, "print data about the best individual")
var dumpValues = flag.Bool("dump", false, "dump a table of actual and fitted values")
var minVal = flag.Float64("min", defaultMinVal, "min x")
var maxVal = flag.Float64("max", defaultMaxVal, "max x")
var step = flag.Float64("step", defaultStep, "step")
var dumpStep = flag.Float64("dumpstep", defaultStep, "dump step")
var perturbation = flag.Float64("perturbe", 0.0, "mag of sample function perturbation")

func main() {
	flag.Parse()

	g := GA.New()

	// the crossover is given by the value in the middle betweet a_i and b_i
	g.CrossOverFunc = func(a GA.Gene, b GA.Gene) GA.Gene {
		v00 := a[0].(float64)
		v01 := a[1].(float64)
		v02 := a[2].(float64)
		v10 := b[0].(float64)
		v11 := b[1].(float64)
		v12 := b[2].(float64)
		return GA.Gene{(v00 + v10) / 2.0, (v01 + v11) / 2.0, (v02 + v12) / 2.0}
	}

	// mutate a "gene"
	g.MutationFunc = func(a GA.Gene) GA.Gene {
		p := rand.Float64()
		if p < mutationProb {
			m1 := (2.0*rand.Float64() - 1.0) / 10.0
			m2 := (2.0*rand.Float64() - 1.0) / 10.0
			m3 := (2.0*rand.Float64() - 1.0) / 10.0
			return GA.Gene{
				a[0].(float64) + m1,
				a[1].(float64) + m2,
				a[2].(float64) + m3,
			}
		}
		return a
	}

	// the generic function we use as fit function
	theFunc := func(x, a, b, c float64) float64 {
		return a*x*x + b*x + c
	}

	// this is used to generate the values; we could (should?)
	// replace it with discrete data points;
	// because of perturbation, let's compute points only once
	fakeData := make(map[float64]float64)

	for x := *minVal; x < *maxVal; x += *step {
		fakeData[x] = theFunc(x, 1, -2, 3) + *perturbation*(2.0*rand.Float64()-1.0)
	}
	actualFunc := func(x float64) float64 {
		if val, ok := fakeData[x]; ok {
			return val
		} else {
			if x > *maxVal || x < *minVal {
				return 0.0 // outside there are no data
			}
			// otherwise x is between defined values: we should find them and interpolate?
			// TODO ...
			// for now this won't happen, so just compute a new value, if we're going to really use it
			return theFunc(x, 1, -2, 3) + *perturbation*(2.0*rand.Float64()-1.0)
		}
	}

	// fitness given by the avg square root distance between each point
	g.CalcFitnessFunc = func(m GA.Gene) float64 {
		c2 := m[0].(float64)
		c1 := m[1].(float64)
		c0 := m[2].(float64)
		s := 0.0
		for x := *minVal; x < *maxVal; x += *step {
			d := actualFunc(x) - theFunc(x, c2, c1, c0)
			s += d * d
		}
		return math.Sqrt(s) / ((*maxVal - *minVal) / *step)
	}

	// generate initial genes; w should be chosen so that
	// expected values are in (-w, w)
	g.ProduceGeneFunc = func() GA.Gene {
		w := 100.0 // the greater the value, the lesser we assume about the correct values
		a := 2.0*rand.Float64() - 1.0
		b := 2.0*rand.Float64() - 1.0
		c := 2.0*rand.Float64() - 1.0
		return GA.Gene{
			a * w, b * w, c * w,
		}
	}

	g.Populate(*initialPop)
	g.Evolve(*iterations)
	res := g.Winner()

	if *printWinner {
		fmt.Printf("data = %v\nfitness = %v\n", res, g.CalcFitnessFunc(res))
	}

	if *dumpValues {
		fmt.Println("x real fitted")
		for x := *minVal; x < *maxVal; x += *dumpStep {
			fmt.Printf("%.5f %.5f %.5f\n", x, actualFunc(x), theFunc(x, res[0].(float64), res[1].(float64), res[2].(float64)))
		}
	}
}
