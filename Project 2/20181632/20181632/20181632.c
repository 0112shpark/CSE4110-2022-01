#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "mysql";
const char* db = "project";

int main(void) {

	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	char buf[1024];
	char buf1[1024];


	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, NULL, 3306, (const char*)NULL, 0);
	mysql_query(connection, "create schema project");
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}
		FILE* fp = fopen("20181632_create.txt", "r");
		int st = 0;
		while (fgets(buf, 1024, fp) != NULL)
		{

			st = mysql_query(connection, buf);
			if (st == 1) {
				//printf("%s", buf);
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			}
		}

		fclose(fp);
		fp = fopen("20181632_insert.txt", "r");
		st = 0;
		while (fgets(buf1, 1024, fp) != NULL)
		{

			st = mysql_query(connection, buf1);
			if (st == 1) {
				printf("%s", buf1);
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			}
		}

		fclose(fp);



		while (1) {
			printf("\n------- SELECT QUERY TYPES -------\n\n");
			printf("\t1. TYPE 1\n");
			printf("\t2. TYPE 2\n");
			printf("\t3. TYPE 3\n");
			printf("\t4. TYPE 4\n");
			printf("\t5. TYPE 5\n");
			printf("\t6. TYPE 6\n");
			printf("\t7. TYPE 7\n");
			printf("\t0. QUIT\n");
			printf("----------------------------------\n");
			int option;
			printf("TYPE OPTION: ");
			scanf("%d", &option);
			const char* query;
			int state;
			int sub_option;
			switch (option) {
				case 0:
				mysql_query(connection, "drop schema project");
				mysql_close(connection);
				exit(0);

			case 1:
				query = "select T.Customer_ID,T.Phone_number from Customer_phone as T, Ship as S where S.stat= 'Destroyed' and S.Customer_ID = T.Customer_ID";
				//select information about customer
				state = mysql_query(connection, query);
				if (state == 0)
				{
					printf("----Destroyed Delivery----\n");
					printf("Customer_ID\tPhone number\n");
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("%s\t\t%s\n", sql_row[0], sql_row[1]);
					}
					mysql_free_result(sql_result);
					while (1) {
						printf("------Type Suboption for TYPE 1 or 0 to go back------\n");
						printf("1-1: \n***Find the contents of that shipment.***\n");
						printf("TYPE OPTION: ");
						scanf("%d", &sub_option);
						if (sub_option == 1) {
							query = "select Tracking_number,Product_name from Ship where Ship.stat= 'Destroyed'";
							//find contents of destroyed shipment
							state = mysql_query(connection, query);
							if (state == 0)
							{
								printf("Contents of the shipment\n");
								printf("Tracking_number\tProduct_name\n");
								sql_result = mysql_store_result(connection);
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
								{
									printf("%s\t\t%s\n", sql_row[0], sql_row[1]);
								}
								mysql_free_result(sql_result);
							}
							int update;
							printf("Want to update?(0->N/1->Y)\n");
							//update tracking number and stat
							scanf("%d", &update);
							if (update == 1) {
								query = "update Ship set Tracking_number = Tracking_number+10 where stat = 'Destroyed'";
								state = mysql_query(connection, query);
								if (state == 1) {

									printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
								}
								query = "update Ship set stat='reshipping' where stat='Destroyed'";
								//stat is reshipping
								state = mysql_query(connection, query);
								if (state == 1) {

									printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
								}
								query = "select * from Ship where stat='reshipping'";
								//print updated tuples
								state = mysql_query(connection, query);
								if (state == 0)
								{
									printf("***Update complete***\n");
									sql_result = mysql_store_result(connection);
									while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
									{
										printf("%s %s %s %s %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
									}
									mysql_free_result(sql_result);
								}
							}

						}
						else if (sub_option == 0) {
							break;
						}
						else
							printf("Wrong Option.\n");
					}

				}
				break;

			case 2:
				query = "select O.amount,P.price, C.C_name from Order_info as O, Product as P, Customer as C where O.Product_name=P.Product_name and O.O_year = '2021'and C.Customer_ID=O.Customer_ID";
				//select dollar used past year per Customer
				state = mysql_query(connection, query);
				if (state == 0)
				{
					int max = 0;
					char name[20][10];
					int used[20];
					int index = 0;
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						strcpy(name[index], sql_row[2]);//store name

						used[index] = atoi(sql_row[0]) * atoi(sql_row[1]);//store price
						index++;
					}
					mysql_free_result(sql_result);
					for (int i = 0; i < index; i++)//eiliminate same name and add
					{
						for (int j = 0; j < index; j++)
						{
							if (!strcmp(name[i], name[j]) && j != i)
							{
								strcpy(name[j], "X");
								used[i] += used[j];
								used[j] = 0;
							}
						}
					}
					for (int i = 1; i < index; i++)//find max
					{
						if (used[i] > used[max])
							max = i;
					}
					
					printf("****Customer who bought the most in 2021*****\n");
					printf("\n------%s $%d------\n\n", name[max], used[max]);
					while (1) {
						printf("------Type Suboption for TYPE 1 or 0 to go back------\n");
						printf("2-1: \n***Product that Customer bought the most in 2021.***\n");
						printf("TYPE OPTION: ");
						scanf("%d", &sub_option);
						if (sub_option == 1) {
							query = "select O.amount,O.Product_name from Order_info as O where O.O_year = '2021'";
							//select unit sales in 2021
							state = mysql_query(connection, query);
							if (state == 0)
							{
								int max = 0;
								char name[20][10];
								int amount[20];
								int index = 0;
								sql_result = mysql_store_result(connection);
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
								{
									strcpy(name[index], sql_row[1]);//store name

									amount[index] = atoi(sql_row[0]);//store unit sales
									index++;
								}
								mysql_free_result(sql_result);
								for (int i = 0; i < index; i++)//eiliminate same name and add
								{
									for (int j = 0; j < index; j++)
									{
										if (!strcmp(name[i], name[j]) && j != i)
										{
											strcpy(name[j], "X");//duplicated name
											amount[i] += amount[j];
											amount[j] = 0;
										}
									}
								}
								for (int i = 1; i < index; i++)//find max
								{
									if (amount[i] > amount[max])
										max = i;
								}
								printf("*****Product that Customer bought the most in 2021*****\n");
								printf("\n------%s %d------\n\n", name[max], amount[max]);
							}
						}
						else if (sub_option == 0) {
							break;
						}
						else
							printf("Wrong Option.\n");

					}
				}
				break;

			case 3:
				query = "select O.amount,P.price, P.Product_name from Order_info as O, Product as P where O.Product_name=P.Product_name and O.O_year = '2021'";
				//select dollar used past year per Product
				state = mysql_query(connection, query);
				if (state == 0)
				{
					int max = 0;
					char name[20][15];
					int used[20];
					int index = 0;
					int count = 0;//#of products sold past year
					int temp;//for sorting
					char temp_ch[10];//for sorting
					sql_result = mysql_store_result(connection);
					printf("------All Products that were sold past year------\n");
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						//printf("%s\n", sql_row[2]);
						strcpy(name[index], sql_row[2]);//store product_name
						used[index] = atoi(sql_row[0]) * atoi(sql_row[1]);//store price
						index++;
					}
					mysql_free_result(sql_result);
					for (int i = 0; i < index; i++)//eiliminate same name and add
					{
						for (int j = 0; j < index; j++)
						{
							if (!strcmp(name[i], name[j]) && j != i)
							{
								strcpy(name[j], "X");
								used[i] += used[j];
								used[j] = 0;
							}
						}
					}
					for (int i = 0; i < index; i++) {//sort by dollar sold
						for (int j = i + 1; j < index; j++) {
							if (used[i] < used[j])
							{
								temp = used[j];
								used[j] = used[i];
								used[i] = temp;

								strcpy(temp_ch, name[j]);
								strcpy(name[j], name[i]);
								strcpy(name[i], temp_ch);
							}
						}
					}
					for (int i = 0; i < index; i++)//print result
					{
						if (strcmp(name[i], "X"))
						{
							count++;
							printf("%s $%d\n", name[i], used[i]);
						}
					}
					while (1) {
						printf("------Type Suboption for TYPE 1, 2 or 0 to go back------\n");
						printf("3-1: ***Top K products sold in 2021(MAX = %d)***\n", count);
						printf("3-2: ***Top 10 percent products sold in 2021***\n");
						printf("TYPE OPTION: ");
						scanf("%d", &sub_option);
						if (sub_option == 1) {
							int k;
							while (1) {//while loop for correct k input

								printf("Enter k (Max %d): ", count);
								scanf("%d", &k);
								if (k >= 0 && k <= count)
									break;
								else
									printf("Wrong boundary\n");
							}
							printf("------Top %d Products------\n", k);
							for (int i = 0; i < k; i++) {
								printf("%s $%d\n", name[i], used[i]);
							}
						}
						else if (sub_option == 2) {
							printf("------Top 10 percents Products------\n");
							float k = (float)count * 0.1;//calculate 10%
							for (int i = 0; i < (int)k + 1; i++) {//make float to int 
								printf("%s $%d\n", name[i], used[i]);
							}
						}
						else if (sub_option == 0)
							break;
						else
							printf("Wrong Option.\n");

					}
				}
				break;

			case 4:
				query = "select O.amount, O.Product_name from Order_info as O where O.O_year = '2021'";
				//select unit sales past year per products
				state = mysql_query(connection, query);
				if (state == 0)
				{
					int max = 0;
					char name[20][15];
					int amount[20];
					int index = 0;
					int count = 0;//#of products sold past year
					int temp;//for sorting
					char temp_ch[10];//for sorting
					sql_result = mysql_store_result(connection);
					printf("------All Products that were sold past year------\n");
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						//printf("%s\n", sql_row[2]);
						strcpy(name[index], sql_row[1]);//store product_name
						amount[index] = atoi(sql_row[0]);//store amount
						index++;
					}
					mysql_free_result(sql_result);
					for (int i = 0; i < index; i++)//eiliminate same name and add
					{
						for (int j = 0; j < index; j++)
						{
							if (!strcmp(name[i], name[j]) && j != i)
							{
								strcpy(name[j], "X");
								amount[i] += amount[j];
								amount[j] = 0;
							}
						}
					}
					for (int i = 0; i < index; i++) {//sort by dollar sold
						for (int j = i + 1; j < index; j++) {
							if (amount[i] < amount[j])
							{
								temp = amount[j];
								amount[j] = amount[i];
								amount[i] = temp;

								strcpy(temp_ch, name[j]);
								strcpy(name[j], name[i]);
								strcpy(name[i], temp_ch);
							}
						}
					}
					for (int i = 0; i < index; i++)//print result
					{
						if (strcmp(name[i], "X"))
						{
							count++;
							printf("%s %d\n", name[i], amount[i]);
						}
					}
					while (1) {
						printf("------Type Suboption for TYPE 1, 2 or 0 to go back------\n");
						printf("4-1: ***Top K products sold in unit sales in 2021(MAX = %d)***\n", count);
						printf("4-2: ***Top 10 percent products sold in unit sales in 2021***\n");
						printf("TYPE OPTION: ");
						scanf("%d", &sub_option);
						if (sub_option == 1) {
							int k;
							while (1) {//while loop for correct k input

								printf("Enter k (Max %d): ", count);
								scanf("%d", &k);
								if (k >= 0 && k <= count)
									break;
								else
									printf("Wrong boundary\n");
							}
							printf("------Top %d Products------\n", k);
							for (int i = 0; i < k; i++) {
								printf("%s %d\n", name[i], amount[i]);
							}
						}
						else if (sub_option == 2) {
							printf("------Top 10 percents Products------\n");
							float k = (float)count * 0.1;//calculate 10%
							for (int i = 0; i < (int)k + 1; i++) {//make float to int 
								printf("%s %d\n", name[i], amount[i]);
							}
						}
						else if (sub_option == 0)
							break;
						else
							printf("Wrong Option.\n");

					}
				}
				break;

			case 5:
				query = "select * from Store_inventory as S where S.S_location = 'California' and S.S_inventory = '0'";
				//select out of stock product_name in California
				state = mysql_query(connection, query);
				if (state == 0)
				{
					printf("-----Out of stock in California -----\n");
					printf("Product_name\tLocation\tInventory\n");
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("%s\t\t%s\t%s\n", sql_row[0], sql_row[1], sql_row[2]);
					}
					mysql_free_result(sql_result);
				}
				break;

			case 6:
				query = "select * from Ship as S where S.stat = 'Late'";
				//select shipments that is out of time
				state = mysql_query(connection, query);
				printf("-----Out of time delivery -----\n");
				printf("Tracking Number\tS_Company\tstatus\t\tCustomer_ID\tProduct name\n");
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("%s\t\t%s\t\t%s\t\t%s\t\t%s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
					}
					mysql_free_result(sql_result);
				}
				query = "select * from Ship as S where S.stat = 'Destroyed'";
				//select shipments that is Destroyed
				state = mysql_query(connection, query);
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("%s\t\t%s\t\t%s\t%s\t\t%s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
					}
					mysql_free_result(sql_result);
				}
				query = "select * from Ship as S where S.stat = 'reshipping'";
				//select shipments that is reshipping due to Destroyed
				state = mysql_query(connection, query);
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("%s\t\t%s\t\t%s\t%s\t\t%s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
					}
					mysql_free_result(sql_result);
				}
				break;

			case 7:
				query = "select O.amount,P.price, C.C_name from Order_info as O, Product as P, Customer as C where O.Product_name=P.Product_name and O.O_year = '2022'and O.O_month = '5' and C.Customer_ID=O.Customer_ID";
				//select dollar used past month per Customer
				state = mysql_query(connection, query);
				if (state == 0)
				{
					int max = 0;
					char name[20][10];
					int used[20];
					int index = 0;
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						strcpy(name[index], sql_row[2]);//store name

						used[index] = atoi(sql_row[0]) * atoi(sql_row[1]);//store price
						index++;
					}
					mysql_free_result(sql_result);
					for (int i = 0; i < index; i++)//eiliminate same name and add
					{
						for (int j = 0; j < index; j++)
						{
							if (!strcmp(name[i], name[j]) && j != i)
							{
								strcpy(name[j], "X");
								used[i] += used[j];
								used[j] = 0;
							}
						}
					}
					query = "select C.C_name, C.Customer_ID from Customer as C";
					//Get Customer info
					state = mysql_query(connection, query);
					int N_index = 0;
					char C_name[20][10];
					int C_ID[20];
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							strcpy(C_name[N_index], sql_row[0]);//store name
							C_ID[N_index] = atoi(sql_row[1]);//store ID
							N_index++;
						}
						mysql_free_result(sql_result);
					}
					query = "select * from Order_info where O_year = '2022' and O_month = '5'";
					//get all order information of past month
					state = mysql_query(connection, query);
					char buy[20][10];//on_off stat
					char P_name[20][10];//Product name
					int C_ID1[20];//Customer_ID
					int year[20];
					int month[20];
					int date[20];
					int amount[20];
					int R_index = 0;
					if (state == 0)
					{
						
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							strcpy(buy[R_index], sql_row[0]);//store stat
							strcpy(P_name[R_index], sql_row[1]);//store product name
							C_ID1[R_index] = atoi(sql_row[2]);
							year[R_index] = atoi(sql_row[3]);
							month[R_index] = atoi(sql_row[4]);
							date[R_index] = atoi(sql_row[5]);
							amount[R_index] = atoi(sql_row[8]);
							R_index++;
						}
						mysql_free_result(sql_result);
					}
					printf("****Bill for 2022-05*****\n");
					
					for (int j = 0; j < N_index; j++) {
						int flag = 0;
						printf("\n-----%s---------\n\n", C_name[j]);
						printf("Product\t\tAmount\tOption\tDate\n");
						for (int k = 0; k < R_index; k++) {
							if (C_ID[j] == C_ID1[k]) {//print bills for each customer
								printf("%s\t\t%d\t%s\t%d-%d-%d\n", P_name[k], amount[k], buy[k], year[k], month[k], date[k]);
								flag = 1;// if there are history
							}
						}
						for (int i = 0; i < index; i++)
						{
							if(!strcmp(C_name[j],name[i]))
								printf("\n\t\t\tTotal : $%d\n\n",used[i]);//print total used
							
						}
						if (!flag)
								printf("\n\tNo Purshase in 2022-05\n\n");
					}
				}
				break;
			}	
		}

		return 0;
	}
}




/*

	printf("------- SELECT QUERY TYPES -------\n\n");
	printf("\t1. TYPE 1\n");
	printf("\t2. TYPE 2\n");
	printf("\t3. TYPE 3\n");
	printf("\t4. TYPE 4\n");
	printf("\t5. TYPE 5\n");
	printf("\t6. TYPE 6\n");
	printf("\t7. TYPE 7\n");
	printf("\t0. QUIT\n");
	//printf("----------------------------------\n");
	printf("\n\n");
	printf("---- TYPE 5 ----\n\n");
	printf("** Find the top k brands by unit sales by the year**\n");
	printf(" Which K? : 3\n");

	return 0;

}
*/