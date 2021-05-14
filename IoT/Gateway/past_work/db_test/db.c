//	1. apt-get install default-libmysqlclient-dev
//	2. mysql_config --cflags 사용하여 mysql.h 경로 찾기
//	3. 찾은 경로를 Makefile 안에 추가
#include <stdio.h>
#include <stdlib.h>	//	exit()
#include "/usr/include/mysql/mysql.h"

int main()
{

	MYSQL		*conn;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	//	hard-coded 되어있는 부분 암호화 해 따로 파일로 관리해야 함
	char		*server		= "2.docker.ai.ourplan.kr";
	char		*user		= "root";
	char		*password	= "aS123123!";
	char		*database	= "KEM";
	int		field;

	conn = mysql_init(NULL);
	if (!mysql_real_connect(conn, server, user, password, NULL, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));

		exit(1);
	}
	puts(res);
	if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));

		exit(1);
	}
	puts("Connect!");

	return 0;
}


