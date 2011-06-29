-- MySQL dump 10.13  Distrib 5.1.49, for debian-linux-gnu (i686)
--
-- Host: localhost    Database: boinc_test
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
-- Table structure for table `result`
--

DROP TABLE IF EXISTS `result`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `result` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `workunitid` int(11) NOT NULL,
  `server_state` int(11) NOT NULL,
  `outcome` int(11) NOT NULL,
  `client_state` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `report_deadline` int(11) NOT NULL,
  `sent_time` int(11) NOT NULL,
  `received_time` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `cpu_time` double NOT NULL,
  `xml_doc_in` blob,
  `xml_doc_out` blob,
  `stderr_out` blob,
  `batch` int(11) NOT NULL,
  `file_delete_state` int(11) NOT NULL,
  `validate_state` int(11) NOT NULL,
  `claimed_credit` double NOT NULL,
  `granted_credit` double NOT NULL,
  `opaque` double NOT NULL,
  `random` int(11) NOT NULL,
  `app_version_num` int(11) NOT NULL,
  `appid` int(11) NOT NULL,
  `exit_status` int(11) NOT NULL,
  `teamid` int(11) NOT NULL,
  `priority` int(11) NOT NULL,
  `mod_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `elapsed_time` double NOT NULL,
  `flops_estimate` double NOT NULL,
  `app_version_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `res_wuid` (`workunitid`),
  KEY `ind_res_st` (`server_state`,`priority`),
  KEY `res_app_state` (`appid`,`server_state`),
  KEY `res_filedel` (`file_delete_state`),
  KEY `res_userid_id` (`userid`,`id`),
  KEY `res_userid_val` (`userid`,`validate_state`),
  KEY `res_hostid_id` (`hostid`,`id`),
  KEY `res_wu_user` (`workunitid`,`userid`)
) ENGINE=InnoDB AUTO_INCREMENT=293 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `result`
--

LOCK TABLES `result` WRITE;
/*!40000 ALTER TABLE `result` DISABLE KEYS */;
INSERT INTO `result` VALUES (283,1304871343,309,5,1,5,3,2,1304957852,1304871452,1304871561,'test_riba_1.txt_1304871319_1_2_0',0.036002,'<file_info>\n    <name>test_riba_1.txt_1304871319_1_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n995df2f9db2958d6608ccd5ab7c16f119696c835c7dece036fddbccbb942077e\n9ae11692921a53693b963a2ca8fbb1be1404c6c32a918d390a2c7e30c44bb81f\n60444013d76e9bfb20c1ad063b80ae463fdcd5cc9731e2d4aca720331f6a72a7\n462ea669ef5d9fcbed603f67cdee165ebfd8b91c561c292de36a1fc7d7445350\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_riba_1.txt_1304871319_1_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_riba_1.txt_1304871319_1_2_0_0</name>\n    <nbytes>273.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>47f2563e42b739742680b300f6501837</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:21:03 (7150): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.167428544544763,0,1366598914,105,1,0,1,0,'2011-05-08 16:19:29',90.284001,801128997.853536,7),(284,1304871343,310,5,1,5,3,2,1304957852,1304871452,1304871513,'test_riba_1.txt_1304871319_2_2_0',0.024001,'<file_info>\n    <name>test_riba_1.txt_1304871319_2_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n73b86a2e17f3e68dd6f160a06efa36ff6f5c091311f7955c292b0598f54f9458\n60c7a4bad158c6ae29d053ee8cbe561b3171e2e4ea8a0967736761fc710c8ab4\n09f559c5493467419bfd141200812ea9804dd8ea0b6bc14a62087494d4942895\nf150a521c7c427b39142fd958d49e3ab4e3430bb786f99b40f41a4c427890e60\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_riba_1.txt_1304871319_2_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_riba_1.txt_1304871319_2_2_0_0</name>\n    <nbytes>153.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>1b321557525ff5330f8de81bf51772ea</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:20:24 (7149): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.0948923445101925,0,838855419,105,1,0,1,0,'2011-05-08 16:18:39',51.169653,801128997.853536,7),(285,1304871343,311,5,1,5,3,2,1304957852,1304871452,1304871891,'test_riba_2.txt_1304871319_1_2_0',0.044002,'<file_info>\n    <name>test_riba_2.txt_1304871319_1_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n1ec3259a441f5c9a4503ac11c08fd4b48f1f2812ccce0901e5f65f5ed21711cd\n0db4174ef9a0477e5eaea2b3037c29c0144dcfda7e59d98b20e6c7af94e1a03b\nd6531df2f73e87e7bcac26640e60358aaf0577a6f6db4b37f0a6dbafbbd159c2\nd32ad32708ea6a8f1510a74b6fe63129ca98477c18fbe53de00263c310cec915\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_riba_2.txt_1304871319_1_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_riba_2.txt_1304871319_1_2_0_0</name>\n    <nbytes>273.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>47f2563e42b739742680b300f6501837</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:26:40 (7221): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.167669413621187,0,611556595,105,1,0,1,0,'2011-05-08 16:24:56',90.413887,801128997.853536,7),(286,1304871344,312,5,1,5,3,2,1304957852,1304871452,1304871798,'test_riba_2.txt_1304871319_2_2_0',0.024001,'<file_info>\n    <name>test_riba_2.txt_1304871319_2_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n4a6baff7916685f81697682fc539095486812f5b7a43b1019d556d8307f5288c\nd65fabd3fdc956ca868811665256d4fea7f7212c9a1a644adeb9c6a850166b05\n434d951ab13f6608ff8626ae540ccdafaf52a70d5c1b4155294e0af1b632e7ee\ne881ad9fe051c97811902cc7c79c62214a2fea2079af4888daad23776fdcd4c4\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_riba_2.txt_1304871319_2_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_riba_2.txt_1304871319_2_2_0_0</name>\n    <nbytes>153.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>1b321557525ff5330f8de81bf51772ea</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:25:08 (7217): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.0949017596303826,0,1240486287,105,1,0,1,0,'2011-05-08 16:23:21',51.17473,801128997.853536,7),(287,1304871344,313,5,1,5,3,2,1304957853,1304871453,1304871809,'test_riba_3.txt_1304871319_1_2_0',0.036001,'<file_info>\n    <name>test_riba_3.txt_1304871319_1_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n6acf5370e0f0c037c9d1ed0751954c38ccdb96e15142f9d51e2b627be3a7b9ca\n536e10e59b675d824ef57f4af85777c833c4aafca403ab5d1f7c7269a8c1960b\n9b261362e9d4c82e37839ef0246c8ef354c9c40cef21422f58e46f39083b2b7b\n001d4aff1ad6f546729ac47e5385369095366365d4cb1a2252f4b455f3a601db\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_riba_3.txt_1304871319_1_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_riba_3.txt_1304871319_1_2_0_0</name>\n    <nbytes>273.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>47f2563e42b739742680b300f6501837</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:25:16 (7214): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.167485749235033,0,676683053,105,1,0,1,0,'2011-05-08 16:23:31',90.314848,801128997.853536,7),(288,1304871344,314,5,1,5,3,2,1304957853,1304871453,1304871723,'test_riba_3.txt_1304871319_2_2_0',0.024001,'<file_info>\n    <name>test_riba_3.txt_1304871319_2_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n908250b191bb2de64b8ae2a9194e029a044da84e27e882925e2baff3a3916644\nd8a62f9b1a421ef286db3a0b8957365b9a41c269d0922c60ea586ea05b0cfe87\n8a548bd3b7991bb63d9084d033cb1270baa9765901e0342552cc550453424414\n66b3c6252a859318c8f901003cf37b1541b032222f9193471d1397434cadcf61\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_riba_3.txt_1304871319_2_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_riba_3.txt_1304871319_2_2_0_0</name>\n    <nbytes>153.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>1b321557525ff5330f8de81bf51772ea</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:23:44 (7201): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.0949766874452652,0,1739390037,105,1,0,1,0,'2011-05-08 16:22:05',51.215134,801128997.853536,7),(289,1304871344,315,5,1,5,3,2,1304957853,1304871453,1304871665,'test_uhc_13.txt_1304871319_1_2_0',0.036001,'<file_info>\n    <name>test_uhc_13.txt_1304871319_1_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n0d7c441e1cf9bc5be400b7b46f39cf6ddad93b490068f67243481d422766a5f4\n4fd5593b2fd0cc3b0e4cf2fc9c9cef6f58cf816b5a99b6efaedd2efb9d68b298\n078099bdd66c5a9121db2211f1b3853d7309272017123286616b6a7b5f698b9c\n87fcf2e2189abad333b78e9083eee87e557cc27626f9fc9e8f07e55829bba804\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_uhc_13.txt_1304871319_1_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_uhc_13.txt_1304871319_1_2_0_0</name>\n    <nbytes>201.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>832a47600b9c2f8ff868c3e214ca7c3b</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:22:52 (7194): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.128725069862426,0,606805082,105,1,0,1,0,'2011-05-08 16:21:15',69.413578,801128997.853536,7),(290,1304871344,316,5,1,5,3,2,1304957853,1304871453,1304871577,'test_uhc_13.txt_1304871319_2_2_0',0.028001,'<file_info>\n    <name>test_uhc_13.txt_1304871319_2_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n129f44412f67dc98a69431cecd1910fc6ceecc1a67d989f7b0dfafa2c1817a3c\nb512c61bd0b89738b6f2a9ad8acd6ccbdc13d67debfa12505b7c13bf2d4d9941\nbab7b9aaf0f681a5edab7594f939bd6a220bb0e3f2ea52f6cbb08749f1663ed1\n42868d7e874c2c9f6753c921d8126b82173dd631a2cb143ea2405c0b68ba45ef\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_uhc_13.txt_1304871319_2_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_uhc_13.txt_1304871319_2_2_0_0</name>\n    <nbytes>177.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>67ebe42a7b303b529a1f18fd5ad39406</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:21:24 (7156): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.109959420507775,0,169522965,105,1,0,1,0,'2011-05-08 16:19:44',59.294408,801128997.853536,7),(291,1304871344,317,5,1,5,3,2,1304957853,1304871453,1304871745,'test_robot_1.txt_1304871319_1_2_0',0.040002,'<file_info>\n    <name>test_robot_1.txt_1304871319_1_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n0e32ffb54b8759524a005bf4ae4f79c9e922bc812b40f95568bf69b9d282c3ee\n2de116dd14fcc929befb566a73bbc08904776777109924f92fedef3a9b95ace1\n62dde7abf343c0156caa1517079887e9685a83c6983e93b3f6c4756dd892677d\naba3da603fd37dc6fed992927e0fd6a39ce47506860eac55c3c8b91c629b5eff\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_robot_1.txt_1304871319_1_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_robot_1.txt_1304871319_1_2_0_0</name>\n    <nbytes>273.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>47f2563e42b739742680b300f6501837</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:24:16 (7199): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.167592763008066,0,1334917215,105,1,0,1,0,'2011-05-08 16:22:30',90.372554,801128997.853536,7),(292,1304871344,318,5,1,5,3,2,1304957853,1304871453,1304871654,'test_robot_1.txt_1304871319_2_2_0',0.028001,'<file_info>\n    <name>test_robot_1.txt_1304871319_2_2_0_0</name>\n    <generated_locally/>\n    <upload_when_present/>\n    <max_nbytes>5000000</max_nbytes>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n<xml_signature>\n990509dbeab4e03b325c745d3931783c168de09094e8ccb2adc0873a7ef2a7cf\nf96862d1dff739e5448257b1fe3af6da29d247cb2bf05072d127732043726265\n1496c00d28c4fb81ddfa98a5647e84a406bdb2dbf051a7037bb6c7f2f677eff9\n30dd68e96aeb77843b70d5096679d8cdc4e0cb8a82f71e6ab93403ebc3bb4bdf\n.\n</xml_signature>\n</file_info>\n<result>\n    <file_ref>\n        <file_name>test_robot_1.txt_1304871319_2_2_0_0</file_name>\n        <open_name>out</open_name>\n    </file_ref>\n</result>\n','<file_info>\n    <name>test_robot_1.txt_1304871319_2_2_0_0</name>\n    <nbytes>153.000000</nbytes>\n    <max_nbytes>5000000.000000</max_nbytes>\n    <md5_cksum>1b321557525ff5330f8de81bf51772ea</md5_cksum>\n    <url>http://172.20.175.53/test_cgi/file_upload_handler</url>\n</file_info>\n','<core_client_version>6.10.58</core_client_version>\n<![CDATA[\n<stderr_txt>\n20:22:45 (7171): called boinc_finish\n\n</stderr_txt>\n]]>\n',0,0,1,0,0.0912054876647166,0,106849470,105,1,0,1,0,'2011-05-08 16:21:00',49.181556,801128997.853536,7);
/*!40000 ALTER TABLE `result` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-05-08 20:26:01
