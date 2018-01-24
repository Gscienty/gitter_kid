import React from 'react';
import BasePage from '../components/BasePage'
import { connect } from 'react-redux';
import { Row, Col, Card, Icon, Button } from 'antd';

class Page extends BasePage {
    componentDidMount() {
        this.props.dispatch({ type: 'repositories/repositories', payload: { name: this.props.match.params.repositoriesName } });
    }
    page() {
        return <div>
            <Row type="flex" gutter={ 32 }>
                {
                    this.props.repositories.repositories
                        .map(repository => <Col xs={ 8 } lg={ 6 } key={ repository }>
                            <Card
                                title={ repository }
                                extra={ <Button shape="circle" icon="right" size="large"
                                    onClick={ e => this.props.history.push(`/repository/${repository}/code`) } /> }>
                                <div style={{ marginBottom: 20 }}>description</div>
                                <div>
                                    <Button style={{ margin: '0 4px 0 0', border: 'none' }}>
                                        <Icon type="star" />
                                        <span style={{ padding: 4 }}>0</span>
                                    </Button>
                                    <Button style={{ margin: '0 4px 0 0', border: 'none' }}>
                                        <Icon type="team" />
                                        <span style={{ padding: 4 }}>0</span>
                                    </Button>
                                </div>
                            </Card>
                        </Col>)
                }
            </Row>
        </div>;
    }
}

export default connect(
    ({ repositories }) => ({ entryUnit: 'repositories', repositories })
)(Page);