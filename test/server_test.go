package test

import (
	"github.com/stretchr/testify/assert"

	"fmt"
	"net"
	"strings"
	"sync"
	"testing"
	"time"
)

func connectToServer(assert *assert.Assertions) *net.TCPConn {
	tcpAddr, err := net.ResolveTCPAddr("tcp", "localhost:8080")
	assert.Nil(err)

	conn, err := net.DialTCP("tcp", nil, tcpAddr)
	assert.Nil(err)
	return conn
}

func runQuery(assert *assert.Assertions, conn *net.TCPConn, query string) string {
	_, err := conn.Write([]byte(query + "\n"))
	assert.Nil(err)

	reply := make([]byte, 512)
	_, err = conn.Read(reply)
	assert.Nil(err)

	return strings.TrimRight(strings.TrimRight(string(reply), "\x00"), "\n")
}

func TestCorrectness(t *testing.T) {
	assert := assert.New(t)

	conn := connectToServer(assert)
	defer conn.Close()

	assert.Equal("NOT FOUND", runQuery(assert, conn, "G key1"))
	assert.Equal("OK", runQuery(assert, conn, "P key1 value"))
	assert.Equal("value", runQuery(assert, conn, "G key1"))
	assert.Equal("NOT FOUND", runQuery(assert, conn, "G key"))
	assert.Equal("DELETED", runQuery(assert, conn, "D key1"))
	assert.Equal("DOESN'T EXISTS", runQuery(assert, conn, "D key1"))
}

func TestLRU(t *testing.T) {
	assert := assert.New(t)

	conn := connectToServer(assert)
	defer conn.Close()

	start := time.Now()

	cnt := 2000
	for x := 0; x < cnt; x += 1 {
		assert.Equal("NOT FOUND", runQuery(assert, conn, fmt.Sprintf("G key%d", x)))
		assert.Equal("OK", runQuery(assert, conn, fmt.Sprintf("P key%d value%d", x, x)))
		assert.Equal(fmt.Sprintf("value%d", x), runQuery(assert, conn, fmt.Sprintf("G key%d", x)))
	}

	for x := 0; x < cnt - 1000; x += 1 {
		assert.Equal("NOT FOUND", runQuery(assert, conn, fmt.Sprintf("G key%d", x)))
	}

	for x := cnt - 1000; x < cnt; x += 1 {
		assert.Equal("DELETED", runQuery(assert, conn, fmt.Sprintf("D key%d", x)))
	}

	cnt_of_operations := int64(cnt * 4)
	assert.Less(time.Since(start).Microseconds() / cnt_of_operations, int64(500))
}

func TestStress(t *testing.T) {
	assert := assert.New(t)

	var wg sync.WaitGroup

	test := func(x int) {
		conn := connectToServer(assert)
		defer conn.Close()

		for i := 0; i < 100; i += 1 {
			assert.Equal("NOT FOUND", runQuery(assert, conn, fmt.Sprintf("G key%d", x)))
			assert.Equal("OK", runQuery(assert, conn, fmt.Sprintf("P key%d value%d", x, x)))
			assert.Equal(fmt.Sprintf("value%d", x), runQuery(assert, conn, fmt.Sprintf("G key%d", x)))
			assert.Equal("NOT FOUND", runQuery(assert, conn, "G key"))
			assert.Equal("DELETED", runQuery(assert, conn, fmt.Sprintf("D key%d", x)))
			assert.Equal("DOESN'T EXISTS", runQuery(assert, conn, fmt.Sprintf("D key%d", x)))
		}

		wg.Done()
	}

	goroutine_cnt := 50
	wg.Add(goroutine_cnt)

	start := time.Now()
	for goroutine_i := 0; goroutine_i < goroutine_cnt; goroutine_i += 1 {
		go test(goroutine_i)
	}
	wg.Wait()

	var cnt_of_operations int64 = 100 * 50 * 6
	assert.Less(time.Since(start).Microseconds() / cnt_of_operations, int64(500))
}
