pipeline {
  agent none

  options {
    skipStagesAfterUnstable()
    skipDefaultCheckout()
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
    stage('build stage') {
      agent {
        docker {
          image 'cross-gcc-windows-x64-sonar-build-wrapper:latest'
          label 'docker && linux'
          args '--memory=1g --memory-swap=1g'
        }
      }

      stages {
        stage('scm stage') {
          steps {
            checkout([
              $class: 'GitSCM',
              branches: scm.branches,
              doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
              extensions: [
                [
                  $class: 'SubmoduleOption',
                  disableSubmodules: false,
                  parentCredentials: true,
                  recursiveSubmodules: true,
                  reference: '',
                  trackingSubmodules: false
                ]
              ],
              submoduleCfg: scm.submoduleCfg,
              userRemoteConfigs: scm.userRemoteConfigs
            ])
          }
        }

        stage("prepare") {
          steps {
            sh "chmod +x ./*.sh"
            sh "./install-dependencies-with-local-conan.sh"
          }
        }

        stage("build") {
          steps {
            sh "rm -rf ${BW_OUTPUT_DIR}"
            sh "mkdir -p ${BW_OUTPUT_DIR}"
            sh "build-wrapper-linux-x86-64 --out-dir ${BW_OUTPUT_DIR} ./build-with-local-cc.sh"
          }
        }

        stage("test") {
          steps {
            sh "./test-coverage-with-local-cc.sh"
          }
        }
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
