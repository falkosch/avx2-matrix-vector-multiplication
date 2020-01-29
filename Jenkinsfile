pipeline {
  agent none
  options {
    skipStagesAfterUnstable()
    timeout(time: 1, unit: 'HOURS')
  }
  triggers {
    pollSCM('H */15 * * *')
  }
  environment {
    CI = true
    HOME = "${env.WORKSPACE}"
    BW_OUTPUT_DIR = 'bw-out'
  }
  stages {
    stage('build') {

      agent {
        docker {
          image 'cross-gcc-windows-x64-sonar-build-wrapper:latest'
          label 'docker && linux'
          args '--memory=1g --memory-swap=1g'
        }
      }

      steps {
        sh "mkdir -p ${BW_OUTPUT_DIR}"
        sh "chmod +x ./*.sh"
        sh "./install-dependencies-with-local-conan.sh"
        sh "build-wrapper-linux-x86-64 --out-dir ${BW_OUTPUT_DIR} ./build-with-local-cc.sh"
      }

    }
    stage('sonar quality gate') {
      agent {
        docker {
          image 'cross-gcc-windows-x64-sonar-scanner-cli:latest'
          label 'docker && linux'
        }
      }
      steps {

        withSonarQubeEnv('sonarqube') {
          sh "sonar-scanner -Dsonar.branch.name=${env.BRANCH_NAME} -Dsonar.cfamily.build-wrapper-output=${BW_OUTPUT_DIR}"
        }

        sleep 3

        timeout(time: 1, unit: 'MINUTES') {
          waitForQualityGate abortPipeline: true
        }

      }
    }
  }
}
