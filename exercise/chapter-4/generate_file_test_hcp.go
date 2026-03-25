package main

import (
	"fmt"
	"math/rand"
	"os"
	"time"
)

func randomString(n int) string {
	const letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	b := make([]byte, n)
	for i := range b {
		b[i] = letters[rand.Intn(len(letters))]
	}
	return string(b)
}

func main() {
	rand.Seed(time.Now().UnixNano())

	fileName := "./exercise/chapter-4/sparse_2mb.txt"

	f, err := os.Create(fileName)
	if err != nil {
		fmt.Println("create file error:", err)
		return
	}
	defer f.Close()

	var currentSize int64 = 0
	targetSize := int64(2 * 1024 * 1024) // ~6MB

	for currentSize < targetSize {

		// 70% seek, 30% write
		if rand.Float64() < 0.7 {

			// seek tiến random (512B -> 64KB)
			hole := int64(rand.Intn(64*1024-512) + 512)

			_, err := f.Seek(hole, 1)
			if err != nil {
				fmt.Println("seek error:", err)
				return
			}

			currentSize += hole

			fmt.Printf("SEEK +%d bytes (total ~%d)\n", hole, currentSize)

		} else {

			// write nhỏ (20–100 bytes)
			n := rand.Intn(80) + 20
			data := randomString(n)

			written, err := f.WriteString(data)
			if err != nil {
				fmt.Println("write error:", err)
				return
			}

			currentSize += int64(written)

			fmt.Printf("WRITE %d bytes (total ~%d)\n", written, currentSize)
		}
	}

	fmt.Println("Done:", fileName)
}