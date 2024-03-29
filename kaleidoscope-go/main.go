package main

import (
	"fmt"
	"os"
)

func main() {
	args := os.Args[1:]
	if len(args) < 1 {
		_, _ = fmt.Fprintf(os.Stderr, "Filename required\n")
		os.Exit(1)
	}
	contents, err := os.ReadFile(args[0])
	if err != nil {
		_, _ = fmt.Fprintf(os.Stderr, "%v\n", err)
		os.Exit(1)
	}
	_, _ = os.Stdout.Write(contents)
}
