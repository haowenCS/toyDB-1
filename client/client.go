package main

import (
	// "net"
	"os"
	"fmt"
	"bufio"
	"strings"
	"strconv"
)

const SERV_IP  = "127.0.0.1"
const VERSION  = "v0.0.1"

func Is_string_type(command string) bool {
	length := len(command)
	if length < 2{
		return false
	}else{
		if command[0] == '"' && command[length - 1] == '"'{
			return true
		}else{
			return false
		}
	}
}

func deal_set(commands []string) (){
	flag := Is_string_type(commands[2])
	for i := 2; i < len(commands); i++{
		if Is_string_type(commands[i]) != flag{
			fmt.Println("ERROR, Values is requested to be correct type")
		}else{
			if !flag{
				num, err := strconv.ParseFloat(commands[i], 64)
				if err != nil{
					fmt.Println("ERROR, Values is requested to be correct type")
				}else{
					fmt.Println("set double:", num)
				}
			}else{
				fmt.Println("set string:", commands[i])
			}
		}
	}
}

func deal_get(commands []string)(){
	fmt.Println("get ", commands[1])
}

func deal_del(commands []string)(){
	fmt.Println("del ", commands[1])
}

func CommandParser(commands []string) (){
	if (commands[0] == "set" || commands[0] == "SET") && len(commands) > 2{
		deal_set(commands)
	}else if(commands[0] == "get" || commands[0] == "GET") && len(commands) == 2{
		deal_get(commands)
	}else if(commands[0] == "del" || commands[0] == "DEL") && len(commands) == 2{
		deal_del(commands)
	}else{
		fmt.Println("cannot parse the command", commands)
	}
}


func main(){
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "Need a port number!")
		os.Exit(1)
	}

	// port := os.Args[1]
	// fmt.Fprintf(os.Stdout, "The server address is %s\n", SERV_IP + ":" + port)
	// ServerAddr, err := net.ResolveTCPAddr("tcp4", SERV_IP + ":" + port)
	// checkError(err)

	// conn, err:=net.DialTCP("tcp", nil, ServerAddr)
	// checkError(err)

	fmt.Fprintf(os.Stdout," _______          _____  ____\n ");  
    fmt.Fprintf(os.Stdout,"|__   __|        |  __ \\|  _ \\                     author : Zhang Yu-qin\n ");
    fmt.Fprintf(os.Stdout,"   | | ___  _   _| |  | | |_) |                    version: %s -go_client\n", VERSION);
    fmt.Fprintf(os.Stdout,"    | |/ _ \\| | | | |  | |  _ <\n");
    fmt.Fprintf(os.Stdout,"    | | (_) | |_| | |__| | |_) |\n");
	fmt.Fprintf(os.Stdout,"    |_|\\___/ \\__, |_____/|____/\n");
    fmt.Fprintf(os.Stdout,"              __/ |\n");         
    fmt.Fprintf(os.Stdout,"             |___/\n");  

	for {
		fmt.Fprintf(os.Stdout, "< ");
		inputReader := bufio.NewReader(os.Stdin)
		input, err := inputReader.ReadString('\n')
		checkError(err)

		commands := strings.Fields(input)
		CommandParser(commands)

	}

	os.Exit(0)
}

func checkError(err error) {
	if err != nil {
		fmt.Fprintf(os.Stderr, "Fatal error: %s\n", err.Error())
		os.Exit(1)
	}
}