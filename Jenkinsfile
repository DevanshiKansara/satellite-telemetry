pipeline {
    agent any

    environment {
        IMAGE = 'satellite-telemetry-ci'
    }

    stages {
        stage('Build Docker image') {
            steps {
                sh 'docker build -t $IMAGE .'
            }
        }

        stage('Static analysis (cppcheck)') {
            steps {
                sh 'docker run --rm $IMAGE cppcheck --enable=warning,performance,portability --error-exitcode=1 -I onboard onboard/'
            }
        }

        stage('Build and test') {
            steps {
                sh '''
                    mkdir -p artifacts
                    docker rm -f telemetry-test || true
                    docker create --name telemetry-test $IMAGE bash -c "./scripts/run_tests.sh && python3 -m pytest tests -q --junitxml=/tmp/pytest-results.xml"
                    docker start -a telemetry-test
                '''
            }
            post {
                always {
                    sh '''
                        docker cp telemetry-test:/tmp/pytest-results.xml artifacts/ || true
                        docker cp telemetry-test:/work/build/onboard artifacts/ || true
                        docker rm -f telemetry-test || true
                    '''
                    junit allowEmptyResults: true, testResults: 'artifacts/pytest-results.xml'
                    archiveArtifacts artifacts: 'artifacts/**', allowEmptyArchive: true
                }
            }
        }
    }
}
