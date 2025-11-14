import aws from 'aws-sdk';
import fs from "fs";
import path from "path";

import pkgJson from '../package.json' with { type: "json" };
import { getOpenSSLPackagePath, getOpenSSLPackageName } from './acquireOpenSSL.mjs';

const s3 = new aws.S3();

const uploadBinaryToS3 = (fileName, bucketName, pathToFile) =>
  s3.upload({
    Body: fs.createReadStream(pathToFile),
    Bucket: bucketName,
    Key: fileName,
    ACL: "public-read"
  }).promise();

export const uploadOpenSSL = async () => {
  const packageName = path.basename(getOpenSSLPackageName());
  const packagePath = getOpenSSLPackagePath();
  console.log(`Uploading ${packagePath} to s3://${pkgJson.binary.bucket_name}/${packageName}`);
  await uploadBinaryToS3(packageName, pkgJson.binary.bucket_name, packagePath);
  const sha256PackageName = `${packageName}.sha256`;
  await uploadBinaryToS3(sha256PackageName, pkgJson.binary.bucket_name, `${packagePath}.sha256`);
};

if (process.argv[1] === import.meta.filename) {
  uploadOpenSSL().catch((error) => {
    console.error('Push to S3 failed: ', error);
    process.exit(1);
  });
}
