#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <limits.h>

typedef struct {
    bool is_verbose;
    char *username;
    char *hostname;
} Query;

int MAX_BUFFER = 512;

void display_query(char *query) {
    char *ptr = query;
    for (int i = 0; i < MAX_BUFFER / 0x10; i++) {
        printf("%p:  ", ptr);
        for (int j = 0; j < 0x10; j++) {
            printf("%02x", (unsigned char) ptr[j]);
            if (j == 7) printf("  ");
        }
        printf("  |");
        for (int j = 0; j < 0x10; j++) {
            if (ptr[j] >= 0x20 && ptr[j] < 0x7f) {
                printf("%c", ptr[j]);
            }
            else printf(".");
        }
        printf("|\n");
        ptr += 0x10; // next memory addr
    }
}

// Get the system's info
// Probably delete this no? (it's not needed since we have another system call)
void display_sys_info() {
    system("lsb_release -a");// Can replace this with whatever other info
}

// Simple linux username validation
bool is_valid_username(char *username) {
    if (username == NULL) return true;
    if (!isalpha(username[0])) return false;

    for (int i = 0; username[i] != '\0'; i++) {
        char c = username[i];
        if (!(islower(c) ||           // 'a' to 'z'
              isdigit(c) ||           // '0' to '9'
              c == '-' ||             // '-'
              c == '_' ||             // '_'
              c == '.')) {            // '.'
            return false;
        }
    }
    return true;
}

// Simple linux username validation
bool is_valid_hostname(char *hostname) {
    return true;
}

// Display a specific user
int display_user(char *username) {
    struct passwd *user_info;
    user_info = getpwnam(username);
    if (user_info != NULL) {
        printf("%s:x:%d:%d:%s:%s:%s\n",
            user_info->pw_name,
            user_info->pw_uid,
            user_info->pw_gid,
            user_info->pw_gecos,
            user_info->pw_dir,
            user_info->pw_shell
        );
        return 0;
    }

    return -1;

    }
}

bool is_current_hostname(char *hostname) {
    char hostname_info[HOST_NAME_MAX + 1];

    if (gethostname(hostname_info, sizeof(hostname_info)) != 0) {
        printf("Couldn't retrieve hostname.\n");
        return false;
    }

    if (strcmp(hostname_info, hostname) == 0) {
        return true;
    }

    return false;
}

void display_hostname(char *hostname) {
    if (is_current_hostname(hostname)) {
        system("uname -a");
    } else {
        printf("Couldn't retrieve hostname '%s'.\n", hostname);
    }
}

void display_all_info() {
    char hostname[HOST_NAME_MAX + 1];

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    } else {
        printf("Couldn't retrieve hostname.\n");
    }

    printf("--------------------\n");

    struct passwd *user_info;
    setpwent();
    while(user_info = getpwent()) {
        if (strcmp(user_info->pw_shell, "/usr/sbin/nologin") != 0 && strcmp(user_info->pw_shell, "/bin/false") != 0) {
            printf("Username: %s\n", user_info->pw_name);
        }
    }
}

void display_user_hostname(char* username, char* hostname) {
    // verbose modekk
    if (is_current_hostname(hostname)) {
        printf("Couldn't find user '%s@%s'.\n", username, hostname);
    }

    display_user(username);
}

int parse_finger_query(char *buffer, Query *user_query) {
    char *username = NULL;
    char *hostname = NULL;
    bool is_verbose = false; // Flag for /W

    // Check for /W for detailed information
    char *ptr = buffer;
    if (strncmp(ptr, "/W", 2) == 0) {
        is_verbose = true;
        ptr += 2; // Move past the /W
        if (*ptr == ' ') {
          ptr++; // Skip space if it exists
        }
        else if (*ptr) {
            printf("Invalid Query - expected only '/W'.\n");
            return -1;
        }
    }

    // Extract username if it exists
    if (*ptr && *ptr != '@') {
        username = ptr;
        while (*ptr && *ptr != ' ' && *ptr != '@') ptr++;
    }

    // Extract hostname if it exists
    if (*ptr == '@') {
        *ptr = '\0';
        hostname = ++ptr;
        while (*ptr && *ptr != ' ' && *ptr != '@') ptr++;
    }

    if (*ptr != '\0') {
        printf("Invalid Query - multiple hostnames not allowed.\n");
        return -1;
    }

    if (username != NULL && !is_valid_username(username)) {
        printf("Invalid Query - bad username '%s'.\n", username);
        return -1;
    }

    if (hostname != NULL && !is_valid_hostname(hostname)) {
        printf("Invalid Query - bad hostname '%s'.\n", hostname);
        return -1;
    }

    user_query->is_verbose = is_verbose;
    user_query->username = username;
    user_query->hostname = hostname;
    return 0;
}

void serve() {
    Query user_query;
    char query[MAX_BUFFER];

    printf("Enter a query (username or empty for all users): ");

    gets(query);
    query[strcspn(query, "\r\n")] = '\0'; // Remove newline character
    printf("\n");

    if (parse_finger_query(query, &user_query) == -1) {
        display_query(query);    
    }

    // Might need to do something with verbose mode?
    if (user_query.username == NULL &&
        user_query.hostname == NULL) {
        // Show all information
        display_all_info(user_query); // do something with verbose mode
    } else if (user_query.username != NULL && user_query.hostname != NULL) {
        display_user_hostname(user_query.username, user_query.hostname);
    } else if (user_query.username != NULL) {
        if (display_user(user_query.username) == -1) {
            printf("User '%s' does not exist on the system.\n", username);
        }
    } else if (user_query.hostname != NULL) {
        display_hostname(user_query.hostname);
    } else {
        printf("Something went horribly wrong...\n");
        display_query(query);
    }
}

void setup() {
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
}

int main(int argc, char *argv[]) {
    setup();
    display_sys_info();
    // display appropriate queries, just show the RFC query format maybe?

    while (true) {
        printf("\n");
        serve();
    }
}

