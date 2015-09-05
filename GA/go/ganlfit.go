package main

// see http://it.mathworks.com/help/optim/examples/nonlinear-data-fitting.html

import (
	"GA"

	"fmt"
	"math"
	"math/rand"
	"os"
)

var dataPoint = []float64{
	0.0000, 5.8955,
	0.1000, 3.5639,
	0.2000, 2.5173,
	0.3000, 1.9790,
	0.4000, 1.8990,
	0.5000, 1.3938,
	0.6000, 1.1359,
	0.7000, 1.0096,
	0.8000, 1.0343,
	0.9000, 0.8435,
	1.0000, 0.6856,
	1.1000, 0.6100,
	1.2000, 0.5392,
	1.3000, 0.3946,
	1.4000, 0.3903,
	1.5000, 0.5474,
	1.6000, 0.3459,
	1.7000, 0.1370,
	1.8000, 0.2211,
	1.9000, 0.1704,
	2.0000, 0.2636,
}

func fitFunc(t, c1, lam1, c2, lam2 float64) float64 {
	return c1*math.Exp(-lam1*t) + c2*math.Exp(-lam2*t)
}

func main() {
	g := GA.New()

	N := len(dataPoint) / 2

	// is this "metric" good in this case? could we leverage the knowledge of how classical
	// more efficient fitting strategies work to choose a better fitness func?
	g.CalcFitnessFunc = func(m GA.Gene) float64 {
		c1 := m[0].(float64)
		lam1 := m[1].(float64)
		c2 := m[2].(float64)
		lam2 := m[3].(float64)
		s := 0.0
		for i := 0; i < N; i++ {
			d := fitFunc(dataPoint[2*i], c1, lam1, c2, lam2) - dataPoint[2*i+1]
			s += d * d
		}
		return s
	}

	// the crossover is given by the value in the middle (is it the best idea in this case?)
	g.CrossOverFunc = func(a GA.Gene, b GA.Gene) GA.Gene {
		var c1, c2, lam1, lam2 [2]float64
		c1[0] = a[0].(float64)
		lam1[0] = a[1].(float64)
		c2[0] = a[2].(float64)
		lam2[0] = a[3].(float64)
		c1[1] = b[0].(float64)
		lam1[1] = b[1].(float64)
		c2[1] = b[2].(float64)
		lam2[1] = b[3].(float64)
		return GA.Gene{
			(c1[0] + c1[1]) / 2.0,
			(lam1[0] + lam1[1]) / 2.0,
			(c2[0] + c2[1]) / 2.0,
			(lam2[0] + lam2[1]) / 2.0,
		}
	}

	// mutate a "gene"; we should adjust the function to make the fit "converge" faster
	g.MutationFunc = func(a GA.Gene) GA.Gene {
		f := [4]float64{a[0].(float64), a[1].(float64), a[2].(float64), a[3].(float64)}
		mutationProb := []float64{0.5, 0.5, 0.5, 0.5}
		for i, v := range mutationProb {
			p := rand.Float64()
			if p < v {
				f[i] += (2.0*rand.Float64() - 1.0) / 10.0
			}
		}
		return GA.Gene{f[0], f[1], f[2], f[3]}
	}

	// generate initial genes; w should be chosen so that
	// expected values are in (-w, w)
	g.ProduceGeneFunc = func() GA.Gene {
		w := 20.0 // the greater the value, the lesser we assume about the correct values
		c1 := 2.0*rand.Float64() - 1.0
		lam1 := 2.0*rand.Float64() - 1.0
		c2 := 2.0*rand.Float64() - 1.0
		lam2 := 2.0*rand.Float64() - 1.0
		return GA.Gene{
			c1 * w, lam1 * w, c2 * w, lam2 * w,
		}
	}

	g.Populate(100)
	g.Evolve(1500)
	r := g.Winner()

	// I obtain
	// [2.9054365850536343 1.4071391340733141 2.9903330149276592 10.678580361408077]
	// compare with 2.8891, 1.4003, 3.0068, 10.5869
	// though the values are reached after fewer iterations (6, 30 ...) :-)
	// conclusions: GA is not good for fitting (when compared to classical methods) ...

	fmt.Fprintf(os.Stderr, "data = %v\n", r)

	fmt.Println("x real fitted")
	for i := 0; i < N; i++ {
		fmt.Printf("%.5f %.5f %.5f\n",
			dataPoint[2*i],
			dataPoint[2*i+1],
			fitFunc(dataPoint[2*i], r[0].(float64), r[1].(float64), r[2].(float64), r[3].(float64)))
	}
}
