-- MySQL dump 10.13  Distrib 5.1.49, for debian-linux-gnu (i686)
--
-- Host: localhost    Database: plankton
-- ------------------------------------------------------
-- Server version	5.1.49-1ubuntu8.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `plankton`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `plankton` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `plankton`;

--
-- Table structure for table `tasks`
--

DROP TABLE IF EXISTS `tasks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tasks` (
  `taskID` int(11) NOT NULL,
  `localID` int(11) DEFAULT NULL,
  `uid` int(11) DEFAULT NULL,
  `updated` datetime DEFAULT NULL,
  `filename` varchar(128) DEFAULT NULL,
  `background` varchar(128) DEFAULT NULL,
  `par1` text,
  `par2` text,
  `par3` text,
  `res1` text,
  `descr` text,
  `status` tinyint(1) DEFAULT NULL,
  `percent` char(3) DEFAULT NULL,
  `calcID` int(16) DEFAULT NULL,
  `startDate` datetime DEFAULT NULL,
  `calcTime` time DEFAULT NULL,
  `ver` varchar(4) DEFAULT NULL,
  `del` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`taskID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tasks`
--

LOCK TABLES `tasks` WRITE;
/*!40000 ALTER TABLE `tasks` DISABLE KEYS */;
INSERT INTO `tasks` VALUES (1,1,1,'2011-01-11 00:00:00','1.txt',NULL,'Wlen=690.0','MinObjSize=0',NULL,NULL,'this is very important task!!!',2,'000',NULL,NULL,NULL,'1.00',0),(2,2,1,'2011-01-11 00:00:00','2.txt',NULL,'Wlen=540.0','MinObjSize=1',NULL,NULL,'this is so... unnecesary',2,'000',NULL,NULL,NULL,'1.00',0),(3,3,1,'2011-01-11 00:00:00','3.txt',NULL,'Wlen=665.0','MinObjSize=0',NULL,NULL,'this is strange wavelenght',2,'000',NULL,NULL,NULL,'1.00',0),(4,1,3,'2011-01-12 00:00:00','1.txt',NULL,NULL,NULL,NULL,NULL,'wtf i dunno lol',2,'000',NULL,NULL,NULL,'1.00',0),(5,1,5,'2011-01-12 00:00:00','1.txt',NULL,NULL,NULL,NULL,NULL,'plz plz plz',1,'042',1,'2011-01-12 00:00:00',NULL,'1.00',0),(6,6,6,'2011-01-12 00:00:00','1.txt',NULL,NULL,NULL,NULL,NULL,'butterfly liberation',0,'100',2,'2011-01-12 00:00:00',NULL,'1.00',0),(7,1,2,'2011-01-12 00:00:00','13.txt',NULL,NULL,NULL,NULL,NULL,NULL,2,'000',6,'2011-01-12 00:00:00',NULL,'1.00',0);
/*!40000 ALTER TABLE `tasks` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `id` int(11) NOT NULL,
  `login` varchar(24) DEFAULT NULL,
  `passwd` varchar(32) DEFAULT NULL,
  `psw` varchar(24) DEFAULT NULL,
  `fname` varchar(24) DEFAULT NULL,
  `mname` varchar(24) DEFAULT NULL,
  `sname` varchar(24) DEFAULT NULL,
  `email` varchar(48) DEFAULT NULL,
  `company` varchar(192) DEFAULT NULL,
  `address` varchar(192) DEFAULT NULL,
  `country` varchar(24) DEFAULT NULL,
  `date` datetime DEFAULT NULL,
  `perm` char(1) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,'riba','a1a1','qwert','Qwert','Deli','Riba','riba@gmail.com','watersource','omks','Rus','2001-01-11 00:00:00','2'),(2,'uhc','a2a2','trewq','Asdf','Latf','Uhc','uhc@gmail.com','watersource','omks','Rus','2001-01-11 00:00:00','1'),(3,'robot','a3a3','trewq','Fdsa','Yiuo','Robot','robot@gmail.com','watersource','omks','Rus','2001-01-11 00:00:00','1'),(4,'verfall','a4a4','qwert','Doiy','Ae','Verfall','verfall@gmail.com','watersource','omks','Rus','2001-01-11 00:00:00','1'),(5,'jesu','a5a5','qwert','Boiu','Ea','Jesu','jesu@gmail.com','watersource','omks','Rus','2001-01-11 00:00:00','1'),(6,'hiha','a6a6','qwert','Le','Sur','Hiha','hiha@gmail.com','watersource','omks','Rus','2002-01-11 00:00:00','3');
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-05-08 13:53:25
