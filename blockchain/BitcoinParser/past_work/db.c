#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

int main(int argc, char *argv[])
{
	MYSQL *conn;

	printf("MySQL Client Version: %s\n", mysql_get_client_info());
	conn = mysql_init(NULL);
	if (!conn) {
		fprintf(stderr, "%s\n", mysql_error(conn));

		return 1;
	}
	/*	접속할 서버명, 사용자이름, 비밀번호, 데이터베이스이름, 포트 번호, 유닉스 소켓, 플래그	*/
	if (!mysql_real_connect(conn, "localhost", "root", "1234", NULL, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);

		return 1;
	}
	if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS testdb")) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);

		return 1;
	}	
	mysql_close(conn);

	return 0;
}

