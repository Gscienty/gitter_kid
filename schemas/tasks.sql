CREATE TABLE IF NOT EXISTS gitterkid_tasks (
	`id` INT PRIMARY KEY AUTO_INCREMENT,
	`executor_username` VARCHAR(32) NOT NULL,
	`assigner_username` VARCHAR(32) NOT NULL,
	`monitor_username` VARCHAR(32),
	`work_group` INT NOT NULL,
	`title` VARCHAR(64) NOT NULL,
	`create_time` INT NOT NULL,
	`completed_time` INT,
	`expired_time` INT NOT NULL,
	`content` BLOB
); 
