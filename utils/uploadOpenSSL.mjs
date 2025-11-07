import aws from 'aws-sdk';
import fs from "fs";
import path from "path";

import pkgJson from './package.json' assert { type: "json" };
import { getOpenSSLPackageName } from './acquireOpenSSL';

const s3 = new aws.S3();

const uploadBinaryToS3 = (binaryName, bucketName, pathToFile) =>
  s3.upload({
    Body: fs.createReadStream(pathToFile),
    Bucket: bucketName,
    Key: binaryName,
    ACL: "public-read"
  }).promise();

export const uploadOpenSSL = async () => {
  const binaryName = getOpenSSLPackageName();
  const pathToFile = path.join(import.meta.dirname, binaryName);
  return uploadBinaryToS3(binaryName, pkgJson.binary.bucket_name, pathToFile);
};

if (require.main === module) {
  uploadOpenSSL().catch((error) => {
    console.error('Push to S3 failed: ', error);
    process.exit(1);
  });
}
