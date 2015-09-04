package main

import (
	"GA"
	"fmt"
	"math"
	"math/rand"
)

func main() {
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
		if p < 0.5 {
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
	// replace it with discrete data points
	actualFunc := func(x float64) float64 {
		return theFunc(x, 1, 2, 3)
	}

	// fitness given by the avg square root distance between each point
	g.CalcFitnessFunc = func(m GA.Gene) float64 {
		c2 := m[0].(float64)
		c1 := m[1].(float64)
		c0 := m[2].(float64)
		s := 0.0
		for x := 0.0; x < 10.0; x += 0.1 {
			d := actualFunc(x) - theFunc(x, c2, c1, c0)
			s += d * d
		}
		return math.Sqrt(s) / (10.0 / 0.1)
	}

	// generate initial genes; w should be chosen so that
	// expected values are in (-w, w)
	g.ProduceGeneFunc = func() GA.Gene {
		w := 5.0
		a := 2.0*rand.Float64() - 1.0
		b := 2.0*rand.Float64() - 1.0
		c := 2.0*rand.Float64() - 1.0
		return GA.Gene{
			a * w, b * w, c * w,
		}
	}

	g.Populate(50)
	g.Evolve(1000)
	res := g.Winner()
	fmt.Println(res, g.CalcFitnessFunc(res))

	for x := 0.0; x < 10.0; x++ {
		fmt.Println(x, actualFunc(x), theFunc(x, res[0].(float64), res[1].(float64), res[2].(float64)))
	}
}
