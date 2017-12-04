import React from 'react';
import BasePage from '../components/BasePage'
import { connect } from 'react-redux';
import { Row, Col, Card, Icon, Button, Tag } from 'antd';

class Page extends BasePage {
    componentDidMount() {
        this.props.dispatch({
            type: 'repository/get',
            payload: { keyword: this.props.match.params.keyword }
        });
    }
    page() {
        return <div>
            <Row type="flex" gutter={ 32 }>
                {
                    this.props.repositories
                        .map(repository => <Col xs={ 8 } lg={ 6 } key={ repository.name }>
                            <Card
                                title={ repository.name }
                                extra={
                                    <Button
                                        shape="circle"
                                        icon="right"
                                        size="large"
                                        onClick={ e => this.props.history.push(`/repository/${repository.name}`) }
                                    />
                                }
                            >
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
    ({ repository }) => ({ repositories: repository.list })
)(Page);